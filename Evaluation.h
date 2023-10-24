#pragma once

#include "Parameters.h"

class Board;

class Evaluation {
public:

    [[nodiscard]] int eval(const Board &board, EvalParameters params) const;

};