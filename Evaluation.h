#pragma once

#include "Parameters.h"
#include "Utils.h"

class Board;

class Evaluation {
public:

    [[nodiscard]] EvalType eval(const Board &board, const EvalParameters& params) const;

};