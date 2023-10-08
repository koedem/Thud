#pragma once

#include "Utils.h"

class Board {

    std::vector<Piece> board;
    Colour to_move;

    /**
     * Number of pieces missing compared to the starting position
     */
    int num_captured;

    /**
     * Points score from dwarf point of view.
     */
    EvalType material;

public:
    explicit Board(Position position);

    void change_to_move();

    void make_move(Move move);

    void unmake_move(Move move);

    Piece get_square(Square square);

    Colour get_to_move();

    void print();

    [[nodiscard]] int number_of_captures() const;

    [[nodiscard]] EvalType get_material() const;

private:
    template<Position TYPE>
    void fill_position();
};