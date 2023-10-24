#include "Evaluation.h"
#include "Board.h"

int Evaluation::eval(const Board& board) const {
    return board.get_material() * 2 + board.get_dwarf_connections();
}