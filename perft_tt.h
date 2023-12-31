#pragma once

#include <cstdint>
#include <vector>
#include <iostream>
#include <fstream>
#include "boost/multiprecision/cpp_int.hpp"
#include "Indexer.h"

class Perft_TT {

    using Index = Indexer::Index;

private:
    static constexpr uint32_t entries_per_bucket = 4;

    struct Entry {
        [[no_unique_address]] Index index;
        Spin_Lock lock;
        uint16_t depth;
        uint64_t value;
    };

    struct alignas(64) Bucket {
        Entry entries[entries_per_bucket];
    };

    void load(const std::string& path) {
        if (perft_store_to_file) {
            std::ifstream file(path, std::ios::in | std::ios::binary);
            if (!file.is_open()) {
                // Handle file open error
                return;
            }
            file.seekg(0, std::ios::end);
            std::streampos fileSize = file.tellg();
            file.seekg(0, std::ios::beg);
            std::cout << fileSize << std::endl;
            file.read(reinterpret_cast<char *>(table.data()), fileSize);
            file.close();
        }
    }

public:
    void store() {
        if (perft_store_to_file) {
            std::ofstream file(storage_path, std::ios::out | std::ios::binary);
            if (!file.is_open()) {
                return;
            }
            file.write(reinterpret_cast<char *>(table.data()), table.size() * sizeof(Bucket));
            file.close();
        }
    }

    Perft_TT() : table(perft_tt_size) {
        load(storage_path);
    }

    void print_size() {
        uint64_t num_elements = 0;
        for (Bucket& bucket : table) {
            for (auto & entry : bucket.entries) {
                if (entry.index.piece_locations != 0) {
                    num_elements++;
                }
            }
        }
        std::cout << "Table elements: " << num_elements << ", missed writes: " << missed_writes << " bucket count "
                  << table.size() << ", bucket capacity: " << table.capacity() << std::endl;
    }

    void emplace(Index index, uint16_t depth, uint64_t value) {
        auto & entries = table[pos(index, depth)].entries;
        bool swapped = false;

        std::lock_guard<Spin_Lock> guard(entries[0].lock);
        for (int i = 0; i < 4; i++) {
            auto & entry = entries[i];
            if (entry.value < value) { // last slot is always replace
                std::swap(entry.value, value);
                std::swap(entry.depth, depth);
                std::swap(entry.index, index);
                swapped = true;
            }
        }
        if (!swapped) {
            missed_writes++;
            auto & entry = entries[2 + (missed_writes & 1)];
            std::swap(entry.value, value);
            std::swap(entry.depth, depth);
            std::swap(entry.index, index);
        }
    }

    uint64_t at(const Index& index, uint16_t depth) {
        auto & entries = table[pos(index, depth)].entries;
        std::lock_guard<Spin_Lock> guard(entries[0].lock);
        for (auto& entry : entries) {
            if (entry.index == index && entry.depth == depth) {
                return entry.value;
            }
        }
        return 0;
    }

    bool contains(const Index& index, uint16_t depth) {
        auto & entries = table[pos(index, depth)].entries;
        std::lock_guard<Spin_Lock> guard(entries[0].lock);
        for (auto& entry : entries) {
            if (entry.index == index && entry.depth == depth) {
                return true;
            }
        }
        return false;
    }

    static inline uint64_t pos(const Index& index, uint16_t depth) {
        return static_cast<uint64_t>((index.piece_locations + index.piece_order + depth) % perft_tt_size); // this is a compile-time constant and gets compiled to either a bit and or an efficient version of this
    }

private:
    std::vector<Bucket> table;

    uint64_t missed_writes = 0;
};