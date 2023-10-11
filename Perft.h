#pragma once


#include "Board.h"
#include "MoveGenerator.h"
#include "perft_tt.h"
#include "Indexer.h"

class Perft {

public:
    enum Perft_Mode {
        NO_HASHING, SIMPLE_HASHING, SYMMETRY_HASHING
    };

private:
    MoveGenerator move_gen;
    Indexer indexer;
    Perft_TT tt;
    uint64_t hash_savings = 0, sub_hash_savings = 0;

    int most_recent_symmetry = 0;

    template<Perft_Mode Mode>
    uint64_t access_tt(Board& board, int depth);

    template<Perft_Mode Mode>
    void store_tt(Board& board, int depth, uint64_t value);

    void print_sub_result(Move move, uint64_t count, uint64_t elapsed_micros) const;

    void print_result(uint64_t count, uint64_t elapsed_micros) const;

public:
    template<Perft_Mode Mode>
    uint64_t hash_perft(Board& board, int depth);

    uint64_t root_perft(Board& board, int depth);
};
