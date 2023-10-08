#pragma once

#include "Board.h"
#include "MoveGenerator.h"

class Search {
    MoveGenerator move_gen;

public:
    EvalType nega_max(Board& board, int depth, int depth_left);
};
