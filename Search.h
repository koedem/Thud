#pragma once

#include <utility>

#include "Board.h"
#include "MoveGenerator.h"

class TranspositionTable;

class Search {

private:
    Board board;
    uint64_t nodes = 0;
    MoveGenerator move_gen;
    TranspositionTable& tt;
    const EvalParameters eval_params;

    EvalType nw_q_search(EvalType beta);

    EvalType null_window_search(uint8_t depth, EvalType beta);

    bool tt_probe(Move &move, EvalType &alpha, EvalType &beta, int depth);

public:
    static int new_depth(int depth, Move move);

    Search(Board board, TranspositionTable& tt, EvalParameters eval_params) : board(std::move(board)), tt(tt),
                                                                              eval_params(eval_params) {};
    EvalType q_search(EvalType alpha, EvalType beta);

    EvalType pv_search(uint8_t depth, EvalType alpha, EvalType beta);

    void reset_nodes();

    [[nodiscard]] uint64_t get_nodes() const;
};
