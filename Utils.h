#pragma once

#include <cstdint>
#include <algorithm>
#include <vector>
#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
#include <atomic>

/**
 * Using values Dwarf = true, Troll = false.
 */
 enum Colour : uint8_t {
     Dwarf = true, Troll = false
 };

Colour flip_colour(Colour c);

std::string to_string(Colour colour);

void print_colour(Colour colour);

using Square = uint8_t;

std::string to_string(Square square);

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
constexpr std::array<Direction, 8> directions = { Direction::nw, Direction::north, Direction::ne, Direction::west,
                                                Direction::east, Direction::sw, Direction::south, Direction::se };

/**
 * Bitboard of the captures of a move. If the move is a dwarf capturing a troll, this will be 1.
 * In case of a troll capturing some dwarfs, the bits are set according to which of the eight directions contained a dwarf.
 * The default value 0 indicates no capture.
 */
using Captures = uint8_t;
constexpr Captures NO_CAPTURES = 0;

using EvalType = int16_t;
constexpr EvalType MIN_EVAL = -10000;
constexpr EvalType MAX_EVAL = 10000;
constexpr EvalType NO_EVAL = -12345;

enum class Piece {
    NONE, DWARF, TROLL, STONE, OUTSIDE
};

enum class Position {
    Full, Endgame
};

void setup_valid_squares();

void setup_center_squares();

bool square_on_board(Square square);

bool square_in_center(Square square);

boost::multiprecision::uint128_t n_choose_k(std::size_t n, std::size_t k);

struct Move {
    Colour to_move;
    Square from;
    Square to;

    /**
     * Bitboard of captures. If capturing piece is a dwarf, this is 1 if a troll was captured, and 0 otherwise.
     * If the capturing piece is a troll, this is a bitboard of the captured neighboring squares in order of directions.
     */
    Captures captures;

    void print() const {
        ::print_colour(to_move);
        ::print_square(from);
        std::cout << "-";
        ::print_square(to);
    }

    bool operator== (const Move& other) const {
        return to_move == other.to_move && from == other.from && to == other.to && captures == other.captures;
    }
};

std::string to_string(Move move);

static constexpr Move NO_MOVE = { Dwarf, 0, 0, 0 };

struct Spin_Lock {
    std::atomic<bool> spin_lock = false;

    void lock() {
        for (;;) {
            if (!spin_lock.exchange(true, std::memory_order_acquire)) {
                break;
            }
            while (spin_lock.load(std::memory_order_relaxed)) {
                //__builtin_ia32_pause(); // In case of hyper-threading this should be beneficial in theory, but in practice it's slightly slower
            }
        }
    }

    void unlock() {
        spin_lock.store(false, std::memory_order_release);
    }

    Spin_Lock() noexcept = default;

    Spin_Lock(Spin_Lock&&) noexcept {
    }

    Spin_Lock& operator=(Spin_Lock&&) noexcept {
        spin_lock = false;
        return *this;
    }
};