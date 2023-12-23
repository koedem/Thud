//
// Created by kolja on 10/7/23.
//

#include <bit>
#include "Search.h"
#include "TranspositionTable.h"
#include "MoveGenerator.h"

EvalType Search::q_search(EvalType alpha, EvalType beta) {
    EvalType q_eval = board.get_eval(eval_params);
    nodes++;

    if (!eval_params.q_search) {
        return q_eval;
    }

    if (q_eval >= beta) {
        return q_eval;
    }
    if (q_eval > alpha) {
        alpha = q_eval;
    }

    std::vector<Move> captures;
    move_gen.generate_captures(captures, board);
    for (auto& capture : captures) {
        board.make_move(capture);
        EvalType inner_eval = -q_search(-beta, -alpha);
        board.unmake_move(capture);
        if (inner_eval > q_eval) {
            q_eval = inner_eval;
            if (q_eval >= beta) {
                break;
            }
            if (q_eval > alpha) {
                alpha = q_eval;
            }
        }
    }

    return q_eval;
}

EvalType Search::nw_q_search(EvalType beta) {
    EvalType q_eval = board.get_eval(eval_params);
    nodes++;
    if (!eval_params.q_search) {
        return q_eval;
    }

    if (q_eval >= beta) {
        return q_eval;
    }

    std::vector<Move> captures;
    move_gen.generate_captures(captures, board);
    for (auto& capture : captures) {
        board.make_move(capture);
        EvalType inner_eval = -nw_q_search(-beta + 1);
        board.unmake_move(capture);
        if (inner_eval > q_eval) {
            q_eval = inner_eval;
            if (q_eval >= beta) {
                break;
            }
        }
    }

    return q_eval;
}

int Search::new_depth(int depth, Move move) {
    if (use_extensions) {
        if (move.captures == 0) {
            return depth - 1;
        } else {
            return depth;
        }
    } else {
        return depth - 1;
    }
}

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

EvalType Search::null_window_search(uint8_t depth, EvalType beta) {
    if (depth == 0) {
        return nw_q_search(beta);
    }

    if (RAZORING && depth == 1) {
        EvalType eval = board.get_eval(eval_params);
        nodes++;
        if (eval >= beta) { // claim that making a move does not make things worse
            board.change_to_move(); // Null move
            EvalType q_eval = -nw_q_search(-beta + 1);
            board.change_to_move();

            if (q_eval >= beta) {
                return eval;
            }
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

    if (depth == 1 && board.get_to_move() == Troll) {
        board.change_to_move();
        std::vector<Move> captures;
        move_gen.generate_captures(captures, board);
        board.change_to_move();

        if (captures.empty()) {
            return nw_q_search(beta);
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
        EvalType inner_eval = -null_window_search(new_depth(depth, move), -beta + 1);
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
        return q_search(alpha, beta);
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
    if (shuffled_pv_search) {
        move_gen.generate_shuffled_moves(moves, board);
    } else {
        move_gen.generate_moves(moves, board);
    }

    int tt_move_index = find(moves, tt_move);
    if (tt_move_index > 0) {
        std::swap(moves[0], moves[tt_move_index]); // Search the TT move first
    }

    bool search_full_window = true;
    for (auto& move : moves) {
        board.make_move(move);
        int inner_depth = new_depth(depth, move);

        EvalType inner_eval;
        if (search_full_window || (inner_eval = -null_window_search(inner_depth, -alpha)) > alpha){
            inner_eval = -pv_search(inner_depth, -beta, -alpha);
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
