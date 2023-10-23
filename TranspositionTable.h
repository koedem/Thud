#pragma once

#include <cstdint>
#include <vector>
#include <iostream>
#include <fstream>
#include "boost/multiprecision/cpp_int.hpp"
#include "Indexer.h"
#include "Search.h"

enum Bound_Type : uint8_t {
    UPPER_BOUND, LOWER_BOUND, EXACT, EVALUATING
};

struct TT_Info {
    EvalType eval = NO_EVAL;
    Move move = NO_MOVE;
    uint8_t depth = 0;
    Bound_Type type = UPPER_BOUND;
    uint8_t proc_number = 0;

    bool operator<(const TT_Info& other) const {
        if (proc_number > 0 && other.proc_number == 0) {
            return false;
        } else if (proc_number == 0 && other.proc_number > 0) {
            return true;
        }
        if (type == EXACT && other.type != EXACT) {
            return false;
        } else if (type != EXACT && other.type == EXACT) {
            return true;
        }
        return depth < other.depth;
    }
};

class TranspositionTable {

    using Index = Indexer::Index;

private:
    static constexpr uint32_t entries_per_bucket = 4;
    static constexpr bool store_to_file = true;
    const std::string storage_path = "/media/kolja/Volume/Test.stt";

    struct Entry {
        [[no_unique_address]] Index index;
        [[no_unique_address]] TT_Info value;
        Spin_Lock lock;

        bool operator<(const Entry& other) const {
            return value < other.value;
        }
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

    explicit TranspositionTable(uint64_t number_of_bits) : size(1 << number_of_bits), table(size) {
        load(storage_path);
    }

    void print_size() const {
        uint64_t num_elements = 0, exact_entries = 0;
        for (const Bucket& bucket : table) {
            for (auto & entry : bucket.entries) {
                if (entry.index.piece_locations != 0) {
                    num_elements++;
                    if (entry.value.type == EXACT) {
                        exact_entries++;
                    }
                }
            }
        }
        std::cout << "Table elements: " << num_elements << ", exact entries: " << exact_entries << ", missed writes: "
            << missed_writes << " bucket count " << table.size() << ", bucket capacity: " << table.capacity() << std::endl;
    }

    void emplace(Index index, TT_Info value) { // TODO check if entry exists, possibly merge
        auto & entries = table[pos(index, value.depth)].entries;
        bool swapped = false;

        std::lock_guard<Spin_Lock> guard(entries[0].lock);
        for (int i = 0; i < 4; i++) {
            auto & entry = entries[i];
            if (entry.value < value) { // last slot is always replace
                std::swap(entry.index, index);
                std::swap(entry.value, value);
                swapped = true;
            }
        }
        if (!swapped) {
            missed_writes++;
            auto & entry = entries[2 + (missed_writes & 1)];
            std::swap(entry.index, index);
            std::swap(entry.value, value);
        }
    }

    TT_Info at(const Index& index, uint16_t depth) {
        auto & entries = table[pos(index, depth)].entries;
        std::lock_guard<Spin_Lock> guard(entries[0].lock);
        for (auto& entry : entries) {
            if (entry.index == index && entry.value.depth == depth) {
                return entry.value;
            }
        }
        return {};
    }

    bool contains(const Index& index, uint16_t depth) {
        auto position = pos(index, depth);
        auto & entries = table[position].entries;
        std::lock_guard<Spin_Lock> guard(entries[0].lock);
        for (auto& entry : entries) {
            if (entry.index == index && entry.value.depth == depth) {
                return true;
            }
        }
        return false;
    }

    void print_pv(Board board, int depth) {
        auto index = board.get_index();
        auto info = at(index, depth);
        while (info.eval != NO_EVAL) {
            info.move.print();
            std::cout << " ";
            board.make_move(info.move);
            index = board.get_index();
            depth = Search::new_depth(depth, info.move);
            info = at(index, depth);
        }
        std::cout << std::endl;
    }

    uint64_t pos(const Index& index, uint16_t depth) {
        return static_cast<uint64_t>((index.piece_locations + index.piece_order + depth) & (size - 1)); // size is a power of two
    }

    void clear() {
        missed_writes = 0;
        table.clear();
        table.resize(size);
    }

private:
    const uint32_t size;
    std::vector<Bucket> table;

    uint64_t missed_writes = 0;
};