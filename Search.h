#pragma once

#include "Board.h"
#include "MoveGenerator.h"
#include "TranspositionTable.h"

static constexpr bool USE_TT = true;

class Search {
    MoveGenerator move_gen;
    TranspositionTable& tt;

public:
    EvalType nega_minimax(Board& board, uint8_t depth);

    explicit Search(TranspositionTable& tt) : tt(tt) {};

    EvalType nega_max(Board &board, uint8_t depth, EvalType alpha, EvalType beta);

    EvalType null_window_search(Board &board, uint8_t depth, EvalType beta);

    EvalType pv_search(Board &board, uint8_t depth, EvalType alpha, EvalType beta);

    bool tt_probe(Board &board, Move &move, EvalType &alpha, EvalType &beta, int depth);
};
