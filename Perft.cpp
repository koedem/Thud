
#include "Perft.h"
#include "Timer.h"

constexpr int tt_depth = 1;

template<>
uint64_t Perft::access_tt<Perft::NO_HASHING>(Board &board, int depth) {
    return 0;
}

template<>
void Perft::store_tt<Perft::NO_HASHING>(Board &board, int depth, uint64_t value) {}

template<>
uint64_t Perft::access_tt<Perft::SYMMETRY_HASHING>(Board &board, int depth) {
    if (depth >= tt_depth) {
        auto index = indexer.symmetric_index(board, most_recent_symmetry);
        index.trolls *= 16;
        index.trolls += depth;
        most_recent_symmetry = index.symmetry;

        uint64_t value = tt.at(index.dwarves, index.trolls); // TODO unify this, use the struct everywhere?
        return value;
    }
    return 0;
}

template<>
void Perft::store_tt<Perft::SYMMETRY_HASHING>(Board &board, int depth, uint64_t value) {
    if (depth >= tt_depth) {
        auto index = indexer.symmetric_index(board, most_recent_symmetry);
        index.trolls *= 16;
        index.trolls += depth;
        most_recent_symmetry = index.symmetry;

        tt.emplace(index.dwarves, index.trolls, value);
    }
}

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
uint64_t Perft::hash_perft(Board &board, int depth) {
    uint64_t tt_access = access_tt<Mode>(board, depth);
    if (tt_access != 0) {
        sub_hash_savings += tt_access;
        return tt_access;
    }

    uint64_t result = 0;

    std::vector<Move> moves;
    move_gen.generate_moves(moves, board);
    if (depth == 1) {
        store_tt<Mode>(board, depth, moves.size());
        return moves.size();
    }

    for (Move move : moves) {
        board.make_move(move);
        result += hash_perft<Mode>(board, depth - 1);
        board.unmake_move(move);
    }

    store_tt<Mode>(board, depth, result);
    return result;
}

void Perft::print_sub_result(Move move, uint64_t count, uint64_t elapsed_micros) const {
    move.print();
    std::cout << " count " << count << " in " << elapsed_micros / 1000 << " ms at speed " << count / (elapsed_micros + 1)
              << " MN/s saving " << (sub_hash_savings * 100) / count << "%" << std::endl;
}

void Perft::print_result(uint64_t count, uint64_t elapsed_micros) const {
    std::cout << std::endl << "Total count " << count << " in " << elapsed_micros / 1000 << " ms at speed "
              << count / (elapsed_micros + 1) << " MN/s" << std::endl; // avoid division by zero
    std::cout << "Hash saving percentage = " << (hash_savings * 100) / count << "%" << std::endl;
}

uint64_t Perft::root_perft(Board& board, int depth) {
    assert(depth > 1);
    uint64_t result = 0;
    hash_savings = 0, sub_hash_savings = 0;
    Timer timer, timer_inside;

    timer.reset();
    std::vector<Move> moves;
    move_gen.generate_moves(moves, board);

    for (Move move : moves) {
        timer_inside.reset();
        board.make_move(move);
        uint64_t count = hash_perft<SYMMETRY_HASHING>(board, depth - 1);
        result += count;
        board.unmake_move(move);

        print_sub_result(move, count, timer_inside.elapsed());
        hash_savings += sub_hash_savings;
        sub_hash_savings = 0;
    }

    uint64_t micros = timer.elapsed();
    print_result(result, micros);
    tt.print_size();
    return result;
}
