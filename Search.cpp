//
// Created by kolja on 10/7/23.
//

#include <bit>
#include "Search.h"

#include "MoveGenerator.h"

EvalType Search::nega_max(Board &board, int depth, int depth_left) {
    if (depth_left == 0) {
        if (board.get_to_move() == Dwarf) {
            return board.get_material();
        } else {
            return -board.get_material();
        }
    }

    EvalType eval = MIN_EVAL;
    std::vector<Move> moves;
    move_gen.generate_moves(moves, board);
    for (auto move : moves) {
        board.make_move(move);

        EvalType inner_eval = -nega_max(board, depth, depth_left - 1);
        if (inner_eval > eval) {
            eval = inner_eval;
        }

        board.unmake_move(move);
    }
    return eval;
}