#pragma once

#include <cstdint>
#include <algorithm>
#include <vector>
#include <iostream>

/**
 * Using values Dwarf = true, Troll = false.
 */
 enum Colour {
     Dwarf = true, Troll = false
 };

Colour flip_colour(Colour c);

void print_colour(Colour colour);

using Square = uint8_t;

void print_square(Square square);

/**
 * Offset of the next square on the board in a given direction.
 */
enum Direction {
    nw = -17, north = -16, ne = -15, west = -1, east = 1, sw = 15, south = 16, se = 17
};

/**
 * List of all directions from north to south and west to east.
 */
const std::vector<Direction> directions = { Direction::nw, Direction::north, Direction::ne, Direction::west,
                                                Direction::east, Direction::sw, Direction::south, Direction::se };

/**
 * Bitboard of the captures of a move. If the move is a dwarf capturing a troll, this will be 1.
 * In case of a troll capturing some dwarfs, the bits are set according to which of the eight directions contained a dwarf.
 * The default value 0 indicates no capture.
 */
using Captures = uint8_t;
constexpr Captures NO_CAPTURES = 0;

using EvalType = int32_t;
constexpr EvalType MIN_EVAL = -10000;
constexpr EvalType MAX_EVAL = 10000;

enum class Piece {
    NONE, DWARF, TROLL, STONE, OUTSIDE
};

enum class Position {
    Full, Endgame
};

bool square_on_board(Square square);

struct Move {
    Colour to_move;
    Square from;
    Square to;

    /**
     * Bitboard of captures. If capturing piece is a dwarf, this is 1 if a troll was captured, and 0 otherwise.
     * If the capturing piece is a troll, this is a bitboard of the captured neighboring squares in order of directions.
     */
    Captures captures;

    void print() {
        ::print_colour(to_move);
        ::print_square(from);
        std::cout << "-";
        ::print_square(to);
    }
};