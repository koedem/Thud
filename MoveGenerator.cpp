#include <cassert>
#include "MoveGenerator.h"

int piece_line_length_in_direction(Board& board, Square square, Direction dir, Piece piece) {
    assert(board.get_square(square) == piece);
    int length = 0;
    while (board.get_square(square) == piece) {
        length++;
        square += dir;
    }
    return length;
}

void add_dwarf_quiets(std::vector<Move>& moves, Board& board, Square square) {
    assert(board.get_square(square) == Piece::DWARF);
    for (int dir = 0; dir < directions.size(); dir++) {
        Square changed = square + directions[dir];
        while (square_on_board(changed) && board.get_square(changed) == Piece::NONE) {
            moves.emplace_back(Move{Dwarf, square, changed, Captures()});
            changed += directions[dir];
        }
    }
}

Captures dwarf_capture_bitboard(Board& board, Square destination_square) {
    return board.get_square(destination_square) == Piece::TROLL;
}

void add_dwarf_captures(std::vector<Move>& moves, Board& board, Square square) {
    assert(board.get_square(square) == Piece::DWARF);

    for (int dir = 0; dir < directions.size(); dir++) {
        int length = piece_line_length_in_direction(board, square, (Direction) -directions[dir], Piece::DWARF); // how far can we capture?

        Square changed = square + directions[dir];
        while (length > 0 && square_on_board(changed)) {
            Captures captures = dwarf_capture_bitboard(board, changed);
            if (captures != NO_CAPTURES) {
                moves.emplace_back(Move{Dwarf, square, changed, Captures(captures)});
                break;
            } else if (board.get_square(changed) != Piece::NONE) {
                break;
            }
            changed += directions[dir];
            length--;
        }
    }
}

Captures troll_capture_bitboard(Board& board, Square destination_square) {
    Captures captures = 0;
    for (int dir = 0; dir < directions.size(); dir++) {
        Square captured = destination_square + directions[dir];
        if (square_on_board(captured) && board.get_square(captured) == Piece::DWARF) {
            captures |= (1 << dir);
        }
    }

    return captures;
}

void add_troll_quiets(std::vector<Move>& moves, Board& board, Square square) {
    assert(board.get_square(square) == Piece::TROLL);
    for (int dir = 0; dir < directions.size(); dir++) {
        Square changed = square + directions[dir];
        if (square_on_board(changed) && board.get_square(changed) == Piece::NONE) {
            if (troll_capture_bitboard(board, changed) == NO_CAPTURES) { // otherwise this is not a quiet move
                moves.emplace_back(Move{Troll, square, changed, Captures()});
            };
        }
    }
}

void add_troll_captures(std::vector<Move>& moves, Board& board, Square square) {
    assert(board.get_square(square) == Piece::TROLL);

    for (int dir = 0; dir < directions.size(); dir++) {
        int length = piece_line_length_in_direction(board, square, (Direction) -directions[dir], Piece::TROLL); // how far can we capture

        Square changed = square + directions[dir];
        while (length > 0 && square_on_board(changed)) {
            if (board.get_square(changed) != Piece::NONE) {
                break;
            }

            int captures = troll_capture_bitboard(board, changed);
            if (captures != NO_CAPTURES) {
                moves.emplace_back((Move{Troll, square, changed, Captures(captures)}));
            }
            changed += directions[dir];
            length--;
        }
    }
}

void generate_dwarf_captures(std::vector<Move>& moves, Board& board) {
    for (Square sq = 0; sq < 255; sq++) {
        if (board.get_square(sq) == Piece::DWARF) {
            add_dwarf_captures(moves, board, sq);
        }
    }
}

void generate_dwarf_quiets(std::vector<Move>& moves, Board& board) {
    for (Square sq = 0; sq < 255; sq++) {
        if (board.get_square(sq) == Piece::DWARF) {
            add_dwarf_quiets(moves, board, sq);
        }
    }
}

void generate_troll_captures(std::vector<Move>& moves, Board& board) {
    for (Square sq = 0; sq < 255; sq++) {
        if (board.get_square(sq) == Piece::TROLL) {
            add_troll_captures(moves, board, sq);
        }
    }
}

void generate_troll_quiets(std::vector<Move>& moves, Board& board) {
    for (Square sq = 0; sq < 255; sq++) {
        if (board.get_square(sq) == Piece::TROLL) {
            add_troll_quiets(moves, board, sq);
        }
    }
}

void MoveGenerator::generate_moves(std::vector<Move>& moves, Board& board) {
    moves.clear();
    if (board.get_to_move() == Dwarf) {
        generate_dwarf_captures(moves, board);
        generate_dwarf_quiets(moves, board);
    } else {
        generate_troll_captures(moves, board);
        generate_troll_quiets(moves, board);
    }
}

void MoveGenerator::generate_captures(std::vector<Move> &captures, Board &board) {
    captures.clear();
    if (board.get_to_move() == Dwarf) {
        generate_dwarf_captures(captures, board);
    } else {
        generate_troll_captures(captures, board);
    }
}