#pragma once


#include <cstdint>
#include "Utils.h"

class AttackBoard {

    std::array<std::array<uint8_t, 8>, 256> line_lengths = {};
    std::array<std::array<uint8_t, 8>, 256> empty_lengths = {};

    std::array<int, 16> controls = {};

    int get_range(const class Board& board, Square sq, int dir_index) const;

public:
    void add_dwarf(const Board& board, Square square, int sign = 1);

    void remove_dwarf(const Board& board, Square square);

    void add_troll(const Board& board, Square square, int sign = 1);

    void remove_troll(const Board& board, Square square);

    [[nodiscard]] const std::array<uint8_t, 8>& get_line_lengths(Square square) const;

    [[nodiscard]] const std::array<uint8_t, 8>& get_empty_spaces(Square square) const;

    AttackBoard& init_lines(const Board& board);

    AttackBoard& init_empties(const Board& board);

    AttackBoard& init_controls(const Board &board);

    const std::array<int, 16>& get_controls() const;

    bool operator==(const AttackBoard& other) const;

};
