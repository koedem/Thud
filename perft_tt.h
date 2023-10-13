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
    static constexpr bool store_to_file = false;
    const std::string storage_path = "/home/kolja/Documents/Programming/C++ programs/Thud/StartPositionD7.tt";

    struct Entry {
        boost::multiprecision::uint128_t dwarves;
        uint64_t trolls;
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
                if (entry.dwarves != 0) {
                    num_elements++;
                }
            }
        }
        std::cout << "Table elements: " << num_elements << ", missed writes: " << missed_writes << " bucket count "
                  << table.size() << ", bucket capacity: " << table.capacity() << std::endl;
    }

    void emplace(boost::multiprecision::uint128_t dwarves, uint64_t trolls, uint64_t value) {
        auto & entries = table[pos(dwarves, trolls)].entries;
        bool swapped = false;
        for (int i = 0; i < 4; i++) {
            auto & entry = entries[i];
            if (entry.value < value) { // last slot is always replace
                std::swap(entry.value, value);
                std::swap(entry.dwarves, dwarves);
                std::swap(entry.trolls, trolls);
                swapped = true;
            }
        }
        if (!swapped) {
            missed_writes++;
            auto & entry = entries[2 + (missed_writes & 1)];
            std::swap(entry.value, value);
            std::swap(entry.dwarves, dwarves);
            std::swap(entry.trolls, trolls);
        }
    }

    uint64_t at(const boost::multiprecision::uint128_t& dwarves, uint64_t trolls) {
        for (auto& entry : table[pos(dwarves, trolls)].entries) {
            if (entry.dwarves == dwarves && entry.trolls == trolls) {
                return entry.value;
            }
        }
        return 0;
    }

    bool contains(const boost::multiprecision::uint128_t& dwarves, uint64_t trolls) {
        for (auto& entry : table[pos(dwarves, trolls)].entries) { // NOLINT(readability-use-anyofallof)
            if (entry.dwarves == dwarves && entry.trolls == trolls) {
                return true;
            }
        }
        return false;
    }

    static inline uint64_t pos(const boost::multiprecision::uint128_t& dwarves, uint64_t trolls) {
        return static_cast<uint64_t>((dwarves + trolls) % size); // this is a compile-time constant and gets compiled to either a bit and or an efficient version of this
    }

private:
    static constexpr uint32_t size = 1 << 27;
    std::vector<Bucket> table;

    uint64_t missed_writes = 0;
};