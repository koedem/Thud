#include "Evaluation.h"
#include "Board.h"

int piece_line_length_in_direction(const Board& board, Square square, Direction dir, Piece piece) {
    assert(board.get_square(square) == piece);
    int length = 0;
    while (board.get_square(square) == piece) {
        length++;
        square += dir;
    }
    return length;
}

int controlled(const Board& board, int length, int dir, Square square) {
    Square changed = square + directions[dir];
    int control = 0;
    while (length > 0 && square_on_board(changed)) {
        if (board.get_square(changed) != Piece::DWARF) {
            control++;
        }
        if (board.get_square(changed) != Piece::NONE) {
            break;
        }
        changed += directions[dir];
        length--;
    }
    return control;
}

int control_choose(int control, const EvalParameters& params) {
    if (control > 1) {
        if (control == 2) {
            return params.control2;
        } else if (control == 3) {
            return params.control3;
        } else if (control > 3) {
            return params.control4;
        }
    }
    return 0;
}

int count_dwarf_control(const Board& board, Square square, const EvalParameters& params) {
    assert(board.get_square(square) == Piece::DWARF);
    int eval_term = 0;
    for (int dir = 0; dir < directions.size(); dir++) {
        int length = piece_line_length_in_direction(board, square, (Direction) -directions[dir], Piece::DWARF); // how far can we capture?

        if (length >= 3) {
            int control = controlled(board, length, dir, square);
            eval_term += control_choose(control, params);
        }
    }
    return eval_term;
}

int old_dwarf_controlled_squares(const Board& board, const EvalParameters& params) {
    int result = 0;
    for (Square sq = 0; sq < 255; sq++) {
        if (board.get_square(sq) == Piece::DWARF) {
            result += count_dwarf_control(board, sq, params);
        }
    }
    return result;
}

int dwarf_controlled_squares(const Board& board, const EvalParameters& params) {
    int result = 0;
    auto control = board.get_controls();
    result += control[2] * params.control2;
    result += control[3] * params.control3;
    for (int i = 0; i < control.size(); i++) {
        result += control[i] * params.control4;
    }

    assert(result == old_dwarf_controlled_squares(board, params));
    return result;
}

EvalType Evaluation::eval(const Board& board, const EvalParameters& params) const {
    int dwarf_control = dwarf_controlled_squares(board, params);
    int material = board.get_dwarf_count() * params.dwarf_factor - board.get_troll_count() * params.troll_factor;
    int dwarf_conns = board.get_dwarf_connections() * params.dwarf_connection_factor;
    int center = (board.get_dwarf_centers() - board.get_troll_centers()) * params.center_factor;
    return material + dwarf_conns + center + dwarf_control;
}