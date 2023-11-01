#include "Evaluation.h"
#include "Board.h"

EvalType Evaluation::eval(const Board& board, const EvalParameters& params) const {
    int material = board.get_dwarf_count() * params.dwarf_factor - board.get_troll_count() * params.troll_factor;
    return material + board.get_dwarf_connections() * params.dwarf_connection_factor;
}