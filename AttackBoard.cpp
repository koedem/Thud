
#include "AttackBoard.h"
#include "Board.h"


int empty_range(const class Board &board, Square square, Direction dir, bool& dwarf_end) {
    int length = 0;
    Square sq = square + dir;
    while (board.get_square(sq) == Piece::NONE) {
        length++;
        sq += dir;
    }
    dwarf_end = board.get_square(sq) == Piece::DWARF;
    return length;
}

int empty_range(const class Board &board, Square square, Direction dir) {
    int length = 0;
    Square sq = square + dir;
    while (board.get_square(sq) == Piece::NONE) {
        length++;
        sq += dir;
    }
    return length;
}

void AttackBoard::remove_dwarf(const Board& board, Square square) {
    for (int dir = 0; dir < directions.size(); dir++) {
        controls[control_lengths[square][dir]] -= 1;
        control_lengths[square][dir] = 0;
    }

    for (int i = 0; i < directions.size() / 2; i++) {
        Direction dir = directions[i];

        int empty_length = 0;
        Square sq = square;
        do {
            sq += dir;
            ++empty_length;
        } while (board.get_square(sq) == Piece::NONE);


        int reverse_empty_length = 0;

        sq = square;
        do {
            sq -= dir;
            ++reverse_empty_length;
        } while (board.get_square(sq) == Piece::NONE);

        sq = square;
        for (int j = 1; j < reverse_empty_length; j++) {
            sq -= dir;
        }
        sq -= dir;
        if (board.get_square(sq) == Piece::DWARF) {
            controls[control_lengths[sq][i]] -= 1;
            control_lengths[sq][i] = 0;
        }

        if (board.get_square(sq) == Piece::DWARF) { // should this work for the 8er?
            int line_length = line_lengths[sq][7 - i] + 1;
            int space = reverse_empty_length + empty_length - 1;
            if (space < line_length && board.get_square(sq + (space + 1) * dir) == Piece::TROLL) {
                ++space; // We can capture a troll
            }
            int range = std::min(line_length, space); // TODO this will be space
            control_lengths[sq][i] = range;
            controls[range] += 1;
        }

        sq = square;
        for (int j = 1; j < empty_length; j++) {
            sq += dir;
        }
        sq += dir;
        if (board.get_square(sq) == Piece::DWARF) {
            controls[control_lengths[sq][7 - i]] -= 1;
            control_lengths[sq][7 - i] = 0;
        }

        if (board.get_square(sq) == Piece::DWARF) {
            int line_length = line_lengths[sq][i] + 1;
            int space = reverse_empty_length + empty_length - 1;
            if (space < line_length && board.get_square(sq - (space + 1) * dir) == Piece::TROLL) {
                ++space; // We can capture a troll
            }
            int range = std::min(line_length, space);

            control_lengths[sq][7 - i] = range;
            controls[range] += 1;
        }


        int line_length = line_lengths[square][i] + 1;
        int reverse_line_length = line_lengths[square][7 - i] + 1;

        sq = square - (reverse_line_length - 1) * dir;
        controls[control_lengths[sq][7 - i]] -= 1;
        control_lengths[sq][7 - i] = 0;

        sq = square + (line_length - 1) * dir;
        controls[control_lengths[sq][i]] -= 1;
        control_lengths[sq][i] = 0;

        for (int j = 1; j <= reverse_line_length; j++) {
            get_line_lengths(square - j * dir)[i] -= line_length;
        }

        for (int j = 1; j <= line_length; j++) {
            get_line_lengths(square + j * dir)[7 - i] -= reverse_line_length;
        }
        sq = square - (reverse_line_length - 1) * dir;

        if (board.get_square(sq) == Piece::DWARF) {
            int ll = line_lengths[sq][i] + 1;
            int space = 0;
            Square temp = sq;
            while (space < ll) {
                temp -= dir;
                if (board.get_square(temp) == Piece::NONE) {
                    space++;
                } else {
                    break;
                }
            }
            if (space < ll && board.get_square(sq - (space + 1) * dir) == Piece::TROLL) {
                ++space; // We can capture a troll
            }
            control_lengths[sq][7 - i] = space;
            controls[space] += 1;
        }

        sq = square + (line_length - 1) * dir;

        if (board.get_square(sq) == Piece::DWARF) {
            int ll = line_lengths[sq][7 - i] + 1;

            int space = 0;
            Square temp = sq;
            while (space < ll) {
                temp += dir;
                if (board.get_square(temp) == Piece::NONE) {
                    ++space;
                } else {
                    break;
                }
            }

            if (space < ll && board.get_square(sq + (space + 1) * dir) == Piece::TROLL) {
                ++space; // We can capture a troll
            }
            control_lengths[sq][i] = space;
            controls[space] += 1;
        }
    }

    if constexpr (assertion_level >= 3) {
        bool passed = verify_control_lengths();
        if (!passed) {
            std::cout << "Failed control length tests in remove_dwarf" << std::endl;
            abort();
        }
    }
    if constexpr (assertion_level >= 4) {
        bool passed = *this == AttackBoard().init_lines(board).init_empties(board).init_controls(board);
        if (!passed) {
            std::cout << "Failed attack board equality test in remove_dwarf" << std::endl;
            abort();
        }
    }
}

/*
 * Use the increased length gained from adding the dwarf to update how far we can capture on either end of its line
 * If line_length is not bigger than 1, then there is no other dwarf on that side of us, so we don't need
 * to do anything here.
 */
void AttackBoard::lengthen_outside_dwarf(const Board &board, int line_length, Direction dir, int i, int square) {
    if (line_length <= 1) {
        return;
    }

    Square sq = square + (line_length - 1) * dir;
    int old_range = control_lengths[sq][i];
    if (old_range < line_length - 1) { // If we are blocked sooner than this, then more length does not help
        return;
    }

    int length = line_lengths[sq][7 - i] + 1; // length behind us, plus our own dwarf
    int space = old_range - 1;
    Square temp = sq + space * dir;
    while (space < length) {
        temp += dir;
        if (board.get_square(temp) == Piece::NONE) {
            space++;
        } else {
            break;
        }
    }

    if (length > space && board.get_square(sq + (space + 1) * dir) == Piece::TROLL) {
        ++space;
    }
    controls[old_range] -= 1;
    control_lengths[sq][i] = space;
    controls[space] += 1;
}

/*
 * We scan the empty lines going out from our just added dwarf. If there is another dwarf at the end of the
 * rainbow it may be blocked by the dwarf that just got added. In that case we need to reduce that other dwarfs
 * control range.
*/
int AttackBoard::shorten_blocked_dwarf(const Board& board, Square square, Direction dir, int i) {
    int empty_length = 0;
    Square sq = square + dir;
    while (board.get_square(sq) == Piece::NONE) {
        ++empty_length;
        sq += dir;
    }

    int length = line_lengths[sq - dir][i];
    if (empty_length < length) {    // Otherwise there's not enough length to reach the added dwarf, so nothing would be blocked
                                    // This implies that there is a dwarf on sq, otherwise the length couldn't be longer
        controls[control_lengths[sq][7 - i]] -= 1;  // So we remove the old value for it
        int space = std::min(length, empty_length);
        control_lengths[sq][7 - i] = space; // and add the new value
        controls[space] += 1;
    }

    if (board.get_square(sq) == Piece::TROLL) {
        ++empty_length;
    }
    return empty_length;
}

/* TODO update doc
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
    for (int i = 0; i < directions.size() / 2; i++) { // Update the line lengths everywhere
        Direction dir = directions[i], reverse_dir = directions[7 - i];
        int line_length = line_lengths[square][i] + 1;
        int reverse_line_length = line_lengths[square][7 - i] + 1;

        for (int j = 1; j <= reverse_line_length; j++) { // On either side of the dwarf, update the line lengths
            get_line_lengths(square - j * dir)[i] += line_length; // I.e. add for each dwarf of the line, the length from the other side + 1
        }

        for (int j = 1; j <= line_length; j++) {
            get_line_lengths(square + j * dir)[7 - i] += reverse_line_length;
        }

        lengthen_outside_dwarf(board, line_length, dir, i, square);
        int space = shorten_blocked_dwarf(board, square, reverse_dir, 7 - i);
        int range = std::min(line_length, space);
        control_lengths[square][7 - i] = range;
        controls[range] += 1;

        lengthen_outside_dwarf(board, reverse_line_length, reverse_dir, 7 - i, square);
        space = shorten_blocked_dwarf(board, square, dir, i);
        range = std::min(reverse_line_length, space);
        control_lengths[square][i] = range;
        controls[range] += 1;
    }

    if constexpr (assertion_level >= 3) {
        bool passed = verify_control_lengths();
        if (!passed) {
            std::cout << "Failed control length tests in add_dwarf" << std::endl;
            abort();
        }
    }
    if constexpr (assertion_level >= 4) {
        bool passed = *this == AttackBoard().init_lines(board).init_controls(board);
        if (!passed) {
            std::cout << "Failed attack board equality test in add_dwarf" << std::endl;
            abort();
        }
    }
}

void AttackBoard::remove_troll(const Board& board, Square square) {
    add_troll(board, square, -1);
}

void AttackBoard::add_troll(const Board &board, Square square, int sign) {
    for (int i = 0; i < directions.size() / 2; i++) {
        Direction dir = directions[i];
        bool forward_dwarf, reverse_dwarf;
        int empty_length = empty_range(board, square, directions[i], forward_dwarf);
        int reverse_empty_length = empty_range(board, square, directions[7 - i], reverse_dwarf);

        if (!reverse_dwarf && !forward_dwarf) {
            continue;
        }

        int space = empty_length + reverse_empty_length + 1;
        Square forward_square = square + empty_length * dir;
        int line_length = line_lengths[forward_square][i];

        int space_here = space;
        if (board.get_square(square - (reverse_empty_length + 1) * directions[i]) == Piece::TROLL) {
            space_here++; // We can capture a troll
        }

        int range = std::min(line_length, space_here); // remove the old long range
        if (sign == -1) {
            get_control_lengths(forward_square + dir)[7 - i] = range;
        }
        controls[range] -= sign;
        range = std::min(line_length, empty_length + 1);
        if (sign == 1) {
            get_control_lengths(forward_square + dir)[7 - i] = range;
        }
        controls[range] += sign;


        Square reverse_square = square - reverse_empty_length * dir;
        line_length = line_lengths[reverse_square][7 - i];

        space_here = space;
        if (board.get_square(square + (empty_length + 1) * directions[i]) == Piece::TROLL) {
            space_here++; // We can capture a troll
        }

        range = std::min(line_length, space_here);
        if (sign == -1) {
            get_control_lengths(reverse_square - dir)[i] = range;
        }
        controls[range] -= sign;
        range = std::min(line_length, reverse_empty_length + 1);
        if (sign == 1) {
            get_control_lengths(reverse_square - dir)[i] = range;
        }
        controls[range] += sign;
    }

    assert(verify_control_lengths());
    assert(*this == AttackBoard().init_lines(board).init_controls(board));
}

/**
 * This function casts the argument to uint8_t which may over or underflow. This maps a negative index to a valid square.
 */
std::array<uint8_t, 8>& AttackBoard::get_line_lengths(Square square) {
    return line_lengths[square];
}

/**
 * This function casts the argument to uint8_t which may over or underflow. This maps a negative index to a valid square.
 */
std::array<uint8_t, 8>& AttackBoard::get_control_lengths(Square square) {
    return control_lengths[square];
}

/**
 * This function casts the argument to uint8_t which may over or underflow. This maps a negative index to a valid square.
 */
const std::array<uint8_t, 8>& AttackBoard::get_line_lengths(Square square) const {
    return line_lengths[square];
}

bool AttackBoard::operator==(const AttackBoard &other) const {
    for (Square square = 0; square < 255; square++) {
        if (!square_on_board(square)) {
            continue;
        }
        for (int i = 0; i < 8; i++) {
            if (line_lengths[square][i] != other.line_lengths[square][i]) {
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
        for (int dir = 0; dir < directions.size(); dir++) {
            int length = full[7 - dir] + 1; // length behind us, plus our own dwarf
            int space = 0;
            Square temp = sq;
            while (space < length) {
                temp += directions[dir];
                if (board.get_square(temp) == Piece::NONE) {
                    space++;
                } else {
                    break;
                }
            }

            if (length > space && board.get_square(sq + (space + 1) * directions[dir]) == Piece::TROLL) {
                ++space; // We can capture a troll
            }
            control_lengths[sq][dir] = space;
            controls[space]++;
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
    return *this;
}

const std::array<int, 16>& AttackBoard::get_controls() const {
    return controls;
}

bool AttackBoard::verify_control_lengths() const {
    std::array<int, 16> control_count = {};
    for (int sq = 0; sq < 256; ++sq) {
        auto square = control_lengths[sq];
        for (auto sq_dir : square) {
            control_count[sq_dir]++;
        }
    }

    bool equal = true;
    for (int i = 2; i < 16; ++i) {
        if (control_count[i] != controls[i]) {
            equal = false;
        }
    }
    return equal;
}
