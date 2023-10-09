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

    template<Perft_Mode Mode>
    uint64_t access_tt(Board& board, int depth);

    template<Perft_Mode Mode>
    void store_tt(Board& board, int depth, uint64_t value);

public:
    __uint128_t perft(Board& board, int depth);

    template<Perft_Mode Mode>
    __uint128_t hash_perft(Board& board, int depth);

};
