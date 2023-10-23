#pragma once

#include <utility>

#include "Board.h"
#include "MoveGenerator.h"

class TranspositionTable;

static constexpr bool USE_TT = true;
static constexpr bool RAZORING = true;

class Search {

private:
    Board board;
    uint64_t nodes = 0;
    MoveGenerator move_gen;
    TranspositionTable& tt;

public:
    static int new_depth(int depth, Move move);

    EvalType nega_minimax(uint8_t depth);

    Search(Board board, TranspositionTable& tt) : board(std::move(board)), tt(tt) {};

    EvalType nega_max(uint8_t depth, EvalType alpha, EvalType beta);

    EvalType null_window_search(uint8_t depth, EvalType beta);

    EvalType pv_search(uint8_t depth, EvalType alpha, EvalType beta);

    bool tt_probe(Move &move, EvalType &alpha, EvalType &beta, int depth);

    void reset_nodes();

    [[nodiscard]] uint64_t get_nodes() const;
};
