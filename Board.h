#pragma once

#include "Utils.h"
#include "Indexer.h"

class Board {

    std::vector<Piece> board;
    Colour to_move;
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

    void print(Move move = NO_MOVE) const;

    [[nodiscard]] int number_of_captures() const;

    [[nodiscard]] EvalType get_material() const;

    [[nodiscard]] EvalType get_eval() const;

    Indexer::Index get_index();

private:
    template<Position TYPE>
    void fill_position();

    void remove_dwarf(Square square);

    void add_dwarf(Square square);

    void remove_troll(Square square);

    void add_troll(Square square);
};