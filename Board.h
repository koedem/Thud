#pragma once

#include "Utils.h"
#include "Indexer.h"
#include "Evaluation.h"
#include "AttackBoard.h"
#include "BigAttackBoard.h"

class Board {

    std::vector<Piece> board;
    Colour to_move;
    int dwarfs_remaining, trolls_remaining;
    int dwarf_connections = 0;
    int center_dwarves, center_trolls;

    Indexer indexer;
    Evaluation evaluation;
    AttackBoard attack_board;
    BigAttackBoard old_ab;

public:
    explicit Board(Position position);

    void change_to_move();

    void make_move(Move move);

    void unmake_move(Move move);

    [[nodiscard]] Piece get_square(Square square) const;

    [[nodiscard]] Colour get_to_move() const;

    void print(Move move = NO_MOVE) const;

    [[nodiscard]] int get_dwarf_count() const;

    [[nodiscard]] int get_troll_count() const;

    [[nodiscard]] EvalType get_eval(const EvalParameters& params) const;

    [[nodiscard]] Indexer::Index get_index();

    [[nodiscard]] int get_dwarf_connections() const;

    [[nodiscard]] int get_dwarf_centers() const;

    [[nodiscard]] int get_troll_centers() const;

    const std::array<uint8_t , 8>& line_lengths(Square square) const;

    const std::array<int, 16>& get_controls() const;

private:
    template<Position TYPE>
    void fill_position();

    void remove_dwarf(Square square);

    void add_dwarf(Square square);

    void remove_troll(Square square);

    void add_troll(Square square);
};