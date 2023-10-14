#pragma once

#pragma once

#include <cstdint>
#include <vector>
#include <iostream>
#include <fstream>
#include "boost/multiprecision/cpp_int.hpp"
#include "Indexer.h"

class Perft_TT {

private:
    static constexpr uint32_t entries_per_bucket = 4;
    static constexpr bool store_to_file = true;
    const std::string storage_path = "/home/kolja/Documents/Programming/C++ programs/Thud/StartPositionD7.tt";

    struct Entry {
        [[no_unique_address]] Indexer::Index index;
        uint64_t value;
    };

    struct alignas(64) Bucket {
        Entry entries[entries_per_bucket];
    };

    void load(const std::string& path) {
        if (store_to_file) {
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
        if (store_to_file) {
            std::ofstream file(storage_path, std::ios::out | std::ios::binary);
            if (!file.is_open()) {
                return;
            }
            file.write(reinterpret_cast<char *>(table.data()), table.size() * sizeof(Bucket));
            file.close();
        }
    }

    Perft_TT() : table(size) {
        load(storage_path);
    }

    void print_size() {
        uint64_t num_elements = 0;
        for (Bucket& bucket : table) {
            for (auto & entry : bucket.entries) {
                if (entry.index.dwarves != 0) {
                    num_elements++;
                }
            }
        }
        std::cout << "Table elements: " << num_elements << ", missed writes: " << missed_writes << " bucket count "
                  << table.size() << ", bucket capacity: " << table.capacity() << std::endl;
    }

    void emplace(Indexer::Index index, uint64_t value) {
        auto & entries = table[pos(index)].entries;
        bool swapped = false;
        for (int i = 0; i < 4; i++) {
            auto & entry = entries[i];
            if (entry.value < value) { // last slot is always replace
                std::swap(entry.value, value);
                std::swap(entry.index, index);
                swapped = true;
            }
        }
        if (!swapped) {
            missed_writes++;
            auto & entry = entries[2 + (missed_writes & 1)];
            std::swap(entry.value, value);
            std::swap(entry.index, index);
        }
    }

    uint64_t at(const Indexer::Index& index) {
        for (auto& entry : table[pos(index)].entries) {
            if (entry.index == index) {
                return entry.value;
            }
        }
        return 0;
    }

    bool contains(const Indexer::Index& index) {
        for (auto& entry : table[pos(index)].entries) { // NOLINT(readability-use-anyofallof)
            if (entry.index == index) {
                return true;
            }
        }
        return false;
    }

    static inline uint64_t pos(const Indexer::Index& index) {
        return static_cast<uint64_t>((index.dwarves + index.trolls) % size); // this is a compile-time constant and gets compiled to either a bit and or an efficient version of this
    }

private:
    static constexpr uint32_t size = 1 << 27;
    std::vector<Bucket> table;

    uint64_t missed_writes = 0;
};