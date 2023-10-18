//
// Created by kolja on 10/7/23.
//

#include <bit>
#include "Search.h"

#include "MoveGenerator.h"

bool Search::tt_probe(Board& board, Move& move, EvalType& alpha, EvalType& beta, int depth) {
    TT_Info tt_entry = tt.at(board.get_index(), depth);
    if (tt_entry.eval != NO_EVAL) {
        assert(tt_entry.depth == depth);
        if (tt_entry.type == EXACT) {
            alpha = tt_entry.eval;
            return true;
        }
        if (tt_entry.type == UPPER_BOUND) {
            beta = std::min(beta, tt_entry.eval);
        } else if (tt_entry.type == LOWER_BOUND) {
            alpha = std::max(alpha, tt_entry.eval);
        }

        if (alpha >= beta) { // Our window is empty due to the TT hit
            alpha = tt_entry.eval;
            return true;
        }
        move = tt_entry.move;
    }
    if (move == NO_MOVE) { // If we didn't find a TT move, try from one depth earlier instead
        tt_entry = tt.at(board.get_index(), depth - 1);
        if (tt_entry.eval != NO_EVAL) {
            assert(tt_entry.depth == depth - 1);
            move = tt_entry.move;
        }
    }
    return false;
}

EvalType Search::nega_minimax(Board &board, uint8_t depth) {
    if (depth == 0) {
        if (board.get_to_move() == Dwarf) {
            return board.get_material();
        } else {
            return -board.get_material();
        }
    }

    if (USE_TT && depth >= 2) {
        EvalType tt_eval = tt.at(indexer.small_index(board), depth).eval;
        if (tt_eval != NO_EVAL) {
            return tt_eval;
        }
    }

    EvalType eval = MIN_EVAL;
    Move best_move;
    std::vector<Move> moves;
    move_gen.generate_moves(moves, board);
    for (auto move : moves) {
        board.make_move(move);

        EvalType inner_eval = -nega_minimax(board, depth - 1);
        if (inner_eval > eval) {
            eval = inner_eval;
            best_move = move;
        }

        board.unmake_move(move);
    }

    if (USE_TT && depth >= 2) {
        auto index = indexer.small_index(board);
        tt.emplace(indexer.small_index(board), {eval, best_move, depth, Bound_Type::EXACT});
    }

    return eval;
}

EvalType Search::nega_max(Board &board, uint8_t depth, EvalType alpha, EvalType beta) {
    if (depth == 0) {
        if (board.get_to_move() == Dwarf) {
            return board.get_material();
        } else {
            return -board.get_material();
        }
    }

    if (USE_TT && depth >= 20) {
        EvalType tt_eval = tt.at(indexer.small_index(board), depth).eval;
        if (tt_eval != NO_EVAL) {
            return tt_eval;
        }
    }

    EvalType eval = MIN_EVAL;
    Move best_move;
    Bound_Type type;
    std::vector<Move> moves;
    move_gen.generate_moves(moves, board);
    for (auto move : moves) {
        board.make_move(move);

        EvalType inner_eval = -nega_max(board, depth - 1, -beta, -alpha);
        board.unmake_move(move);
        if (inner_eval > eval) {
            eval = inner_eval;
            best_move = move;
            if (eval >= beta) {
                type = LOWER_BOUND;
                break;
            }
            if (eval > alpha) {
                alpha = eval;
                type = EXACT;
            }
        }

    }

    if (USE_TT && depth >= 2 && type == EXACT) {
        tt.emplace(indexer.small_index(board), {eval, best_move, depth, type});
    }

    return eval;
}