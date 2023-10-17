#pragma once

#include "Board.h"
#include <random>

class MoveGenerator {

    const uint32_t seed;
public:
    void generate_shuffled_moves(std::vector<Move>& moves, Board& board) const;

    void generate_moves(std::vector<Move>& moves, Board& board) const;

    void generate_captures(std::vector<Move>& captures, Board& board) const;

    explicit MoveGenerator(uint32_t seed = std::random_device()()) : seed(seed) {
    }
};