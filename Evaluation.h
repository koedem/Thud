#pragma once

class Board;

class Evaluation {
public:
    [[nodiscard]] int eval(const Board &board) const;

};