#pragma once

#include <random>

int quiet_selfplay(int dwarf_depth, int troll_depth, const EvalParameters dwarf_eval, EvalParameters troll_eval) {
    Board board(Position::Full);
    TranspositionTable tt(num_bits);

    auto loop_condition = [&](int depth, uint64_t time) {
        if (board.get_to_move() == Dwarf) {
            return depth <= dwarf_depth;
        } else {
            return depth <= troll_depth;
        }
    };

    for (int i = 0; i < game_length; i++) {
        Search search(board, tt, board.get_to_move() == Dwarf ? dwarf_eval : troll_eval);
        Timer timer;
        int depth = 1;
        for (; loop_condition(depth, timer.elapsed()); depth++) {
            timer.reset();
            search.pv_search(depth, MIN_EVAL, MAX_EVAL);
        }
        auto move = tt.at(board.get_index(), depth - 1).move;
        board.make_move(move);
        tt.clear();
    }
    return board.get_dwarf_count() - board.get_troll_count() * 4;
}

void new_dwarf_clop(int argc, char** argv, int depth, int offset) {
    std::string dwarf_dwarfs(argv[4]), dwarf_control3(argv[6]), dwarf_control4(argv[8]), dwarf_conn(argv[10]);
    int d_d = std::stoi(dwarf_dwarfs), d_c3 = std::stoi(dwarf_control3), d_c4 = std::stoi(dwarf_control4);
    int d_conn = std::stoi(dwarf_conn);

    EvalParameters new_dwarf = {d_d, 400, d_conn, 1, 0, d_c3, d_c4, true};

    std::uniform_int_distribution<int> dist(-32, 32);
    std::random_device rand;
    int32_t random_offset = dist(rand);
    int result = 0;
    for (int i = 0; i < 5; i++) {
        result += quiet_selfplay(depth + offset, depth, new_dwarf, default_troll_eval);
    }
    result += random_offset;

    if (result == 0) {
        std::cout << "D" << std::endl;
    } else if (result > 0) {
        std::cout << "W" << std::endl;
    } else {
        std::cout << "L" << std::endl;
    }
}
