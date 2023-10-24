#include "Evaluation.h"
#include "Board.h"

int Evaluation::eval(const Board& board, EvalParameters params) const {
    int material = board.get_dwarf_count() - board.get_troll_count() * 4;
    return material * 2 + board.get_dwarf_connections();
}