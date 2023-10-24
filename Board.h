#pragma once

#include "Utils.h"
#include "Indexer.h"
#include "Evaluation.h"

class Board {

    std::vector<Piece> board;
    Colour to_move;
    int dwarfs_remaining, trolls_remaining;
    int dwarf_connections = 0;

    Indexer indexer;
    Evaluation evaluation;

public:
    explicit Board(Position position);

    void change_to_move();

    void make_move(Move move);

    void unmake_move(Move move);

    [[nodiscard]] Piece get_square(Square square) const;

    [[nodiscard]] Colour get_to_move() const;

    void print(Move move = NO_MOVE) const;

    [[nodiscard]] EvalType get_material() const;

    [[nodiscard]] EvalType get_eval() const;

    [[nodiscard]] Indexer::Index get_index();

    [[nodiscard]] int get_dwarf_connections() const;

private:
    template<Position TYPE>
    void fill_position();

    void remove_dwarf(Square square);

    void add_dwarf(Square square);

    void remove_troll(Square square);

    void add_troll(Square square);
};