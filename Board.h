#pragma once

#include "Utils.h"
#include "Indexer.h"

class Board {

    std::vector<Piece> board;
    Colour to_move;

    /**
     * Number of pieces missing compared to the starting position
     */
    int num_captured;

    int dwarfs_remaining, trolls_remaining;

    int dwarf_connections = 0;

    Indexer indexer;

public:
    explicit Board(Position position);

    void change_to_move();

    void make_move(Move move);

    void unmake_move(Move move);

    [[nodiscard]] Piece get_square(Square square) const;

    [[nodiscard]] Colour get_to_move() const;

    void print() const;

    void print(Move move) const;

    [[nodiscard]] int number_of_captures() const;

    [[nodiscard]] EvalType get_material() const;

    [[nodiscard]] EvalType get_eval() const;

    Indexer::SmallIndex get_index();

private:
    template<Position TYPE>
    void fill_position();

    void remove_dwarf(Square square);

    void add_dwarf(Square square);
};