//
// Created by kolja on 10/7/23.
//

#include <bit>
#include "Search.h"

#include "MoveGenerator.h"


constexpr int find(const std::vector<Move>& moves, Move move) {
    for (int i = 0; i < moves.size(); i++) {
        if (moves[i] == move) {
            return i;
        }
    }
    return -1;
}

bool Search::tt_probe(Move& move, EvalType& alpha, EvalType& beta, int depth) {
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

EvalType Search::nega_minimax(uint8_t depth) {
    if (depth == 0) {
        nodes++;
        if (board.get_to_move() == Dwarf) {
            return board.get_eval();
        } else {
            return -board.get_eval();
        }
    }

    if (USE_TT && depth >= 2) {
        EvalType tt_eval = tt.at(board.get_index(), depth).eval;
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

        EvalType inner_eval = -nega_minimax(depth - 1);
        if (inner_eval > eval) {
            eval = inner_eval;
            best_move = move;
        }

        board.unmake_move(move);
    }

    if (USE_TT && depth >= 2) {
        auto index = board.get_index();
        tt.emplace(index, {eval, best_move, depth, Bound_Type::EXACT});
    }

    return eval;
}

EvalType Search::nega_max(uint8_t depth, EvalType alpha, EvalType beta) {
    if (depth == 0) {
        nodes++;
        if (board.get_to_move() == Dwarf) {
            return board.get_eval();
        } else {
            return -board.get_eval();
        }
    }

    if (USE_TT && depth >= 2) { // TODO
        auto tt_info =  tt.at(board.get_index(), depth);
        EvalType tt_eval = tt_info.eval;
        if (tt_eval != NO_EVAL && tt_info.type == EXACT) {
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

        EvalType inner_eval = -nega_max(depth - 1, -beta, -alpha);
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
        tt.emplace(board.get_index(), {eval, best_move, depth, type});
    }

    return eval;
}

EvalType Search::null_window_search(uint8_t depth, EvalType beta) {
    if (depth == 0) {
        nodes++;
        if (board.get_to_move() == Dwarf) {
            return board.get_eval();
        } else {
            return -board.get_eval();
        }
    }

    if (RAZORING && depth == 1) {
        EvalType eval;
        if (board.get_to_move() == Dwarf) {
            eval = board.get_eval();
        } else {
            eval = -board.get_eval();
        }
        if (eval >= beta) { // claim that making a move does not make things worse
            nodes++;
            return eval;
        }
    }

    Move tt_move = NO_MOVE;
    EvalType alpha = beta - 1;
    EvalType eval = MIN_EVAL;
    if (USE_TT && depth >= 2) { // TODO
        if (tt_probe(tt_move, alpha, beta, depth)) {
            return alpha;
        }
    }

    Bound_Type type = UPPER_BOUND;
    std::vector<Move> moves;
    move_gen.generate_moves(moves, board);

    int tt_move_index = find(moves, tt_move);
    if (tt_move_index > 0) {
        std::swap(moves[0], moves[tt_move_index]);
    }

    for (auto& move : moves) {
        board.make_move(move);

        EvalType inner_eval = -null_window_search(depth - 1, -beta + 1);
        board.unmake_move(move);
        if (inner_eval > eval) {
            eval = inner_eval;
            tt_move = move;
            if (eval >= beta) {
                type = LOWER_BOUND;
                break;
            }
        }
    }

    if (USE_TT && depth >= 2) { // TODO
        tt.emplace(board.get_index(), {eval, tt_move, depth, type});
    }

    return eval;
}

EvalType Search::pv_search(uint8_t depth, EvalType alpha, EvalType beta) {
    if (depth == 0) {
        nodes++;
        if (board.get_to_move() == Dwarf) {
            return board.get_eval();
        } else {
            return -board.get_eval();
        }
    }

    Move tt_move = NO_MOVE;
    if (USE_TT && depth >= 2) { // TODO
        if (tt_probe(tt_move, alpha, beta, depth)) {
            return alpha;
        }
    }
    EvalType eval = MIN_EVAL;
    Bound_Type type = UPPER_BOUND;
    std::vector<Move> moves;
    move_gen.generate_moves(moves, board);

    int tt_move_index = find(moves, tt_move);
    if (tt_move_index > 0) {
        std::swap(moves[0], moves[tt_move_index]); // Search the TT move first
    }

    bool search_full_window = true;
    for (auto& move : moves) {
        board.make_move(move);

        EvalType inner_eval;
        if (search_full_window || (inner_eval = -null_window_search(depth - 1, -alpha)) > alpha){
            inner_eval = -pv_search(depth - 1, -beta, -alpha);
            search_full_window = false;
        }
        board.unmake_move(move);

        if (inner_eval > eval) {
            eval = inner_eval;
            tt_move = move; // If it stays this way, this is the best move
            if (eval >= beta) {
                type = LOWER_BOUND;
                break;
            }
            if (eval > alpha) {
                alpha = eval;
                type = EXACT; // We raised alpha, so it's no longer a lower bound, either exact or upper bound
            }

        }
    }

    if (USE_TT && depth >= 2 || type == EXACT) {
        tt.emplace(board.get_index(), {eval, tt_move, depth, type});
    }
    return eval;
}

void Search::reset_nodes() {
    nodes = 0;
}

uint64_t Search::get_nodes() const {
    return nodes;
}
