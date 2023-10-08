#pragma once

#include "Board.h"

class MoveGenerator {

public:
    void generate_moves(std::vector<Move>& moves, Board& board);

    void generate_captures(std::vector<Move>& captures, Board& board);
};