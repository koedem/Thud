#pragma once

#pragma once

#include <cstdint>
#include <vector>
#include <iostream>
#include "boost/multiprecision/cpp_int.hpp"

class Perft_TT {

private:
    static constexpr uint32_t entries_per_bucket = 4;

    struct Entry {
        boost::multiprecision::uint128_t dwarves;
        uint64_t trolls;
        uint64_t value;
    };

    struct alignas(64) Bucket {
        Entry entries[entries_per_bucket];
    };



public:
    Perft_TT() : table(size) {
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
        auto & entries = table[pos(dwarves)].entries;
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
        for (auto& entry : table[pos(dwarves)].entries) {
            if (entry.dwarves == dwarves && entry.trolls == trolls) {
                return entry.value;
            }
        }
        return 0;
    }

    bool contains(const boost::multiprecision::uint128_t& dwarves, uint64_t trolls) {
        for (auto& entry : table[pos(dwarves)].entries) { // NOLINT(readability-use-anyofallof)
            if (entry.dwarves == dwarves && entry.trolls == trolls) {
                return true;
            }
        }
        return false;
    }

    static inline uint64_t pos(const boost::multiprecision::uint128_t& dwarves) {
        return static_cast<uint64_t>(dwarves % size); // this is a compile-time constant and gets compiled to either a bit and or an efficient version of this
    }

private:
    static constexpr uint32_t size = 1 << 0;
    std::vector<Bucket> table;

    uint64_t missed_writes = 0;
};