
#include "Perft.h"
#include "Timer.h"

template<>
uint64_t Perft::access_tt<Perft::NO_HASHING>(Board &board, int depth) {
    return 0;
}

template<>
void Perft::store_tt<Perft::NO_HASHING>(Board &board, int depth, uint64_t value) {}

template<>
uint64_t Perft::access_tt<Perft::SYMMETRY_HASHING>(Board &board, int depth) {
    if (depth >= perft_tt_depth) {
        auto index = board.get_index();
        uint64_t value = tt.at(index, depth); // TODO unify this, use the struct everywhere?
        return value;
    }
    return 0;
}

template<>
void Perft::store_tt<Perft::SYMMETRY_HASHING>(Board &board, int depth, uint64_t value) {
    if (depth >= perft_tt_depth) {
        auto index = board.get_index();
        tt.emplace(index, depth, value);
    }
}

template<>
uint64_t Perft::access_tt<Perft::SIMPLE_HASHING>(Board& board, int depth) {
    auto index = board.get_index();
    uint64_t value = tt.at(index, depth);
    return value;
}

template<>
void Perft::store_tt<Perft::SIMPLE_HASHING>(Board& board, int depth, uint64_t value) {
    auto index = board.get_index();
    tt.emplace(index, depth, value);
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
    if (depth == 1) {
        move_gen.generate_moves(moves, board);
        store_tt<Mode>(board, depth, moves.size());
        return moves.size();
    }
    move_gen.generate_shuffled_moves(moves, board);

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

uint64_t Perft::root_perft(Board board, int depth, bool print) {
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

        if (print) {
            if (timer_inside.elapsed() > 1000000) { // Storing is not worth if we did not spend at least a second of actual calculation
                //tt.store();
            }
            print_sub_result(move, count, timer_inside.elapsed());
        }
        hash_savings += sub_hash_savings;
        sub_hash_savings = 0;
    }

    if (print) {
        uint64_t micros = timer.elapsed();
        print_result(result, micros);
        tt.print_size();
    }
    return result;
}
