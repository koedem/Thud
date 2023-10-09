
#include "Perft.h"

template<>
uint64_t Perft::access_tt<Perft::NO_HASHING>(Board &board, int depth) {
    return 0;
}

template<>
void Perft::store_tt<Perft::NO_HASHING>(Board &board, int depth, uint64_t value) {}

template<>
uint64_t Perft::access_tt<Perft::SYMMETRY_HASHING>(Board &board, int depth) {
    return 0;
}

template<>
void Perft::store_tt<Perft::SYMMETRY_HASHING>(Board &board, int depth, uint64_t value) {}

template<>
uint64_t Perft::access_tt<Perft::SIMPLE_HASHING>(Board& board, int depth) {
    boost::multiprecision::uint128_t dwarf_index = indexer.index_dwarves(board);
    uint64_t troll_index = indexer.index_trolls(board);
    troll_index *= 16;
    troll_index += depth;

    uint64_t value = tt.at(dwarf_index, troll_index);
    return value;
}

template<>
void Perft::store_tt<Perft::SIMPLE_HASHING>(Board& board, int depth, uint64_t value) {
    boost::multiprecision::uint128_t dwarf_index = indexer.index_dwarves(board);
    uint64_t troll_index = indexer.index_trolls(board);
    troll_index *= 16;
    troll_index += depth;

    tt.emplace(dwarf_index, troll_index, value);
}

template<Perft::Perft_Mode Mode>
__uint128_t Perft::hash_perft(Board &board, int depth) {
    uint64_t tt_access = access_tt<Mode>(board, depth);
    if (tt_access != 0) {
        return tt_access;
    }

    __uint128_t result = 0;

    std::vector<Move> moves;
    move_gen.generate_moves(moves, board);
    if (depth == 1) {
        store_tt<Mode>(board, depth, moves.size());
        return moves.size();
    }

    for (Move move : moves) {
        board.make_move(move);
        result += perft(board, depth - 1);
        board.unmake_move(move);
    }

    store_tt<Mode>(board, depth, (uint64_t) result);
    return result;
}

__uint128_t Perft::perft(Board &board, int depth) {
    __uint128_t result = 0;

    std::vector<Move> moves;
    move_gen.generate_moves(moves, board);
    if (depth == 1) {
        return moves.size();
    }

    for (Move move : moves) {
        board.make_move(move);
        result += perft(board, depth - 1);
        board.unmake_move(move);
    }

    return result;
}
