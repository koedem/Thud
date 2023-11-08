//
// Created by kolja on 11/7/23.
//

#include "AttackBoard.h"
#include "Board.h"

void AttackBoard::remove_dwarf(const Board& board, Square square) {
    for (int dir = 0; dir < directions.size(); dir++) { // TODO unify with get_range
        int length = line_lengths[square][dir] + 1; // length behind us, plus our own dwarf
        int space = empty_lengths[square][7 - dir];
        if (length > space && board.get_square(square - (space + 1) * directions[dir]) == Piece::TROLL) {
            space++; // We can capture a troll
        }
        int range = std::min(length, space);
        controls[range] -= 1;
    }

    for (int i = 0; i < directions.size() / 2; i++) {
        Direction dir = directions[i];

        int empty_length = empty_lengths[square][i] + 1;
        int reverse_empty_length = empty_lengths[square][7 - i] + 1;

        Square sq = square;
        for (int j = 1; j < reverse_empty_length; j++) {
            sq -= dir;
            empty_lengths[sq][i] += empty_length;
        }
        sq -= dir;
        if (board.get_square(sq) == Piece::DWARF) {
            int range = get_range(board, sq, i);
            controls[range] -= 1;
        }

        empty_lengths[sq][i] += empty_length;

        if (board.get_square(sq) == Piece::DWARF) { // should this work for the 8er?
            int range = get_range(board, sq, i);
            controls[range] += 1;
        }

        sq = square;
        for (int j = 1; j < empty_length; j++) {
            sq += dir;
            empty_lengths[sq][7 - i] += reverse_empty_length;
        }
        sq += dir;
        if (board.get_square(sq) == Piece::DWARF) {
            int range = get_range(board, sq, 7 - i);
            controls[range] -= 1;
        }

        empty_lengths[sq][7 - i] += reverse_empty_length;

        if (board.get_square(sq) == Piece::DWARF) {
            int range = get_range(board, sq, 7 - i);
            controls[range] += 1;
        }


        int line_length = line_lengths[square][i] + 1;
        int reverse_line_length = line_lengths[square][7 - i] + 1;

        int range = get_range(board, square - (reverse_line_length - 1) * dir, 7 - i);
        controls[range] -= 1;
        range = get_range(board, square + (line_length - 1) * dir, i);
        controls[range] -= 1;

        for (int j = 1; j <= reverse_line_length; j++) {
            line_lengths[square - j * dir][i] -= line_length;
        }

        for (int j = 1; j <= line_length; j++) {
            line_lengths[square + j * dir][7 - i] -= reverse_line_length;
        }
        range = get_range(board, square - (reverse_line_length - 1) * dir, 7 - i);
        controls[range] += 1;
        range = get_range(board, square + (line_length - 1) * dir, i);
        controls[range] += 1;
    }

    assert(*this == AttackBoard().init_lines(board).init_empties(board).init_controls(board));
}

int AttackBoard::get_range(const Board& board, Square sq, int dir_index) const {
    if (board.get_square(sq) != Piece::DWARF) {
        return 0;
    }

    Direction dir = directions[dir_index];
    int space = empty_lengths[sq][dir_index];
    int line_length = line_lengths[sq][7 - dir_index] + 1;
    if (board.get_square(sq + (space + 1) * dir) == Piece::TROLL) {
        space++; // We can capture a troll
    }
    int range = std::min(line_length, space);
    return range;
}

/*
 * Control procedure for adding a dwarf: for each direction, remove the neighbor control line that is now blocked by us
 * Also remove the far end that will get lengthened. Then update the line lengths in that direction.
 * Then add at the opposite end of this the new longer range.
 *
 * Then, if we block a far away dwarf line, remove that from control and add a shorter one.
 * Finally, primitively add the new dwarf ranges that we added for the piece itself.
 *
 * For removing:
 *
 * Primitively remove the new dwarf range that we removed.
 *
 * Also remove the far end that will get shortened. Then update the line lengths in that direction.
 * Then add at the opposite end of this the new shorter range.
 * Finally, if we unblock a far away dwarf line, remove that shorter line and add a longer one.
 *
 */
void AttackBoard::add_dwarf(const Board& board, Square square) {
    for (int i = 0; i < directions.size() / 2; i++) {
        Direction dir = directions[i];
        int line_length = line_lengths[square][i] + 1;
        int reverse_line_length = line_lengths[square][7 - i] + 1;

        int range = get_range(board, square - (reverse_line_length - 1) * dir, 7 - i);
        controls[range] -= 1;
        range = get_range(board, square + (line_length - 1) * dir, i);
        controls[range] -= 1;

        for (int j = 1; j <= reverse_line_length; j++) {
            line_lengths[square - j * dir][i] += line_length;
        }

        for (int j = 1; j <= line_length; j++) {
            line_lengths[square + j * dir][7 - i] += reverse_line_length;
        }
        range = get_range(board, square - (reverse_line_length - 1) * dir, 7 - i);
        controls[range] += 1;
        range = get_range(board, square + (line_length - 1) * dir, i);
        controls[range] += 1;

        int empty_length = empty_lengths[square][i] + 1;
        int reverse_empty_length = empty_lengths[square][7 - i] + 1;

        Square sq = square;
        for (int j = 1; j < reverse_empty_length; j++) {
            sq -= dir;
            empty_lengths[sq][i] -= empty_length;
        }
        sq -= dir;
        if (board.get_square(sq) == Piece::DWARF) {
            int range = get_range(board, sq, i);
            controls[range] -= 1;
        }

        empty_lengths[sq][i] -= empty_length;

        if (board.get_square(sq) == Piece::DWARF) {
            int range = get_range(board, sq, i);
            controls[range] += 1;
        }

        sq = square;
        for (int j = 1; j < empty_length; j++) {
            sq += dir;
            empty_lengths[sq][7 - i] -= reverse_empty_length;
        }
        sq += dir;

        if (board.get_square(sq) == Piece::DWARF) {
            int range = get_range(board, sq, 7 - i);
            controls[range] -= 1;
        }

        empty_lengths[sq][7 - i] -= reverse_empty_length;

        if (board.get_square(sq) == Piece::DWARF) {
            int range = get_range(board, sq, 7 - i);
            controls[range] += 1;
        }
    }

    for (int dir = 0; dir < directions.size(); dir++) { // TODO unify with get_range
        int length = line_lengths[square][dir] + 1; // length behind us, plus our own dwarf
        int space = empty_lengths[square][7 - dir];
        if (length > space && board.get_square(square - (space + 1) * directions[dir]) == Piece::TROLL) {
            space++; // We can capture a troll
        }
        int range = std::min(length, space);
        controls[range] += 1;
    }

    assert(*this == AttackBoard().init_lines(board).init_empties(board).init_controls(board));
}

void AttackBoard::remove_troll(const Board& board, Square square) {
    add_troll(board, square, -1);
}

void AttackBoard::add_troll(const Board &board, Square square, int sign) {
    for (int i = 0; i < directions.size() / 2; i++) {
        Direction dir = directions[i];
        int empty_length = empty_lengths[square][i] + 1;
        int reverse_empty_length = empty_lengths[square][7 - i] + 1;

        for (int j = 1; j <= reverse_empty_length; j++) {
            Square sq = square - j * dir;

            if (board.get_square(sq) == Piece::DWARF) {
                int space = empty_lengths[sq][i];
                int line_length = line_lengths[sq][7 - i] + 1;
                if (board.get_square(sq + (space + 1) * dir) == Piece::TROLL || sq + (space + 1) * dir == square) {
                    space++; // We can capture a troll
                }
                int range = std::min(line_length, space);
                controls[range] -= sign;
            }

            empty_lengths[sq][i] -= empty_length * sign;

            if (board.get_square(sq) == Piece::DWARF) {
                int space = empty_lengths[sq][i];
                int line_length = line_lengths[sq][7 - i] + 1;
                if (board.get_square(sq + (space + 1) * dir) == Piece::TROLL || sq + (space + 1) * dir == square) { // this better evaluate to true!
                    space++; // We can capture a troll
                }
                int range = std::min(line_length, space);
                controls[range] += sign;
            }
        }

        for (int j = 1; j <= empty_length; j++) {
            Square sq = square + j * dir;

            if (board.get_square(sq) == Piece::DWARF) {
                int space = empty_lengths[sq][7 - i];
                int line_length = line_lengths[sq][i] + 1;
                if (board.get_square(sq - (space + 1) * dir) == Piece::TROLL || sq - (space + 1) * dir == square) {
                    space++; // We can capture a troll
                }
                int range = std::min(line_length, space);
                controls[range] -= sign;
            }

            empty_lengths[sq][7 - i] -= reverse_empty_length * sign;

            if (board.get_square(sq) == Piece::DWARF) {
                int space = empty_lengths[sq][7 - i];
                int line_length = line_lengths[sq][i] + 1;
                if (board.get_square(sq - (space + 1) * dir) == Piece::TROLL || sq - (space + 1) * dir == square) {
                    space++; // We can capture a troll
                }
                int range = std::min(line_length, space);
                controls[range] += sign;
            }
        }
    }

    assert(*this == AttackBoard().init_lines(board).init_empties(board).init_controls(board));
}

const std::array<uint8_t, 8>& AttackBoard::get_line_lengths(Square square) const {
    return line_lengths[square];
}

const std::array<uint8_t, 8>& AttackBoard::get_empty_spaces(Square square) const {
    return empty_lengths[square];
}

bool AttackBoard::operator==(const AttackBoard &other) const {
    for (Square square = 0; square < 255; square++) {
        if (!square_on_board(square)) {
            continue;
        }
        for (int i = 0; i < 8; i++) {
            Direction dir = directions[i];
            if (line_lengths[square][i] != other.line_lengths[square][i]) {
                return false;
            }
            if (empty_lengths[square][i] != other.empty_lengths[square][i]) {
                return false;
            }
        }
    }

    for (int i = 2; i < 16; i++) {
        if (controls[i] != other.controls[i]) {
            return false;
        }
    }
    return true;
}

AttackBoard& AttackBoard::init_controls(const Board& board) {
    for (int i = 0; i < 16; i++) {
        controls[i] = 0;
    }

    for (Square sq = 0; sq < 255; sq++) {
        if (board.get_square(sq) != Piece::DWARF) {
            continue;
        }

        auto full = get_line_lengths(sq);
        auto empty = get_empty_spaces(sq);
        for (int dir = 0; dir < directions.size(); dir++) {
            int length = full[dir] + 1; // length behind us, plus our own dwarf
            int space = empty[7 - dir];
            if (board.get_square(sq - (space + 1) * directions[dir]) == Piece::TROLL) {
                space++; // We can capture a troll
            }
            int range = std::min(length, space);
            controls[range]++;
        }
    }
    return *this;
}

AttackBoard& AttackBoard::init_lines(const Board& board) {
    for (Square square = 0; square < 255; square++) {
        if (!square_on_board(square)) {
            continue;
        }

        for (int i = 0; i < 8; i++) {
            Direction dir = directions[i];

            Square sq = square + dir;
            int length = 0;
            while (board.get_square(sq) == Piece::DWARF) {
                length++;
                sq += dir;
            }
            line_lengths[square][i] = length;
        }
    }
    return *this;
}

AttackBoard& AttackBoard::init_empties(const Board& board) {
    for (Square square = 0; square < 255; square++) {
        if (!square_on_board(square)) {
            continue;
        }

        for (int i = 0; i < 8; i++) {
            Direction dir = directions[i];

            Square sq = square + dir;
            int length = 0;
            while (board.get_square(sq) == Piece::NONE) {
                length++;
                sq += dir;
            }
            /*if (board.get_square(sq) == Piece::TROLL) { // TODO
                length++; // Dwarfs can capture trolls, so let's count that as movable-to space.
            }*/

            empty_lengths[square][i] = length;
        }
    }
    return *this;
}

const std::array<int, 16>& AttackBoard::get_controls() const {
    return controls;
}
