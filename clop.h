#pragma once

#include <random>

int mocked[6][200] = { {}, {}, {}, { -26, -26, -4, -2, -3, 0, -6, -9, -2, -1, -4, -6, -3, -27, -5, -5, -4, -5, 0, -8, -22, -1, -26, 0, -22, -2, -4, -12,
                     -3, -2, -26, -27, -4, -8, -27, -27, -1, -3, -3, -5, -5, -26, -11, -6, -1, -26, -27, -5, -2, -2, -26, -5, -4, -27, -5, -26, -2, -7,
                     -8, -1, -27, -3, -27, -6, -9, -26, -6, -9, -7, -10, -6, -3, -5, -2, -10, -2, 1, -27, -8, -5, -18, -5, -5, -8, -7, -26, -27, -4, -3,
                     -4, -6, -4, -1, -3, -8, -16, -22, -3, -7, -26, -6, -4, -7, -4, -10, -3, -4, -6, -7, -6, -27, -10, -4, -27, -27, -3, -27, -5, -5, 1,
                     -1, -5, -2, -26, -26, -22, -7, -22, 0, -3, -2, -5, -26, -6, -6, -2, -4, -2, -7, -4, -26, -27, -27, -26, -7, -5, -5, -10, -2, -4,
                     -10, -4, -5, -5, -7, -2, -27, -2, -8, -4, -26, -27, -26, -7, -1, -2, -27, -27, -3, -5, -27, -26, -26, -8, -1, -4, -2, -2, -26, -26,
                     -5, -6, -10, -4, -2, -7, -5, -26, -1, -4, -5, -5, -4, -8, -5, -2, -27, -5, -7, -1  },
                       { 2, -5, -1, -6, -6, 0, -2, -1, -2, -2, -6, 1, 1, 2, -4, 4, -2, -4, -3, -2, -22, -1, 0, -26, -8, -7, -9, -3, 0, -3, -1, -5, -4,
                     -4, -1, -5, -2, -23, 0, -3, -12, -5, 1, 2, -5, -1, -4, -1, -6, -4, -3, 1, -4, -7, -2, -3, -22, -4, -3, -5, -2, -4, -26, -3, -27, 0,
                     -2, -6, 2, -2, -3, -5, -26, 0, 0, 7, -6, -6, -5, -14, -6, -4, -3, -6, -6, -6, -4, -3, 0, -4, -5, -7, -18, -18, -3, 1, -4, -3, -5, 2,
                     -2, 0, -3, -4, 0, 2, -2, -26, -2, -2, -26, -2, -26, -12, -26, -5, -1, -2, -14, -9, -1, -3, -3, -3, -6, -6, 2, 0, -1, -3, -2, -26,
                     -5, 1, -27, 0, -3, -8, -27, 3, -4, -27, -26, 2, -27, -7, -22, -27, 4, -2, -3, -5, -26, -4, -6, 0, -5, -4, -3, -1, -3, -5, -4, -26,
                     -4, -2, -8, -6, -26, 1, 1, -3, -3, -3, -3, -4, -2, -26, -4, -1, -3, 0, -4, -2, -2, -3, -1, -5, 0, -2, -4, -5, 0, -5, -7, 5, -4, -26,
                     0, -8 },
                       { -12, -4, -8, 5, -3, -7, -4, 0, -6, -8, -8, -5, -2, -1, -1, -7, -5, 0, 2, -2, -6, -1, -6, 4, -9, -1, -4, -3, -4, -6, -6, -5, -3,
                         -1, 3, 3, 1, -4, 2, 0, 4, -4, -4, -1, -3, 4, 0, -1, -5, -1, -3, -1, -4, -6, -4, -2, -1, -2, -7, 0, -2, -3, -4, 2, -2, -17, 0, 1,
                         -11, -2, -3, 2, -1, -6, 0, -4, -6, -3, -3, -2, 1, -5, -4, 0, 2, -5, -4, -2, -3, -3, -3, -1, -3, -5, 0, -2, -5, 1, -1, -7, -2,
                         -5, 1, -5, -3, -4, -9, -2, 0, -2, -1, 0, -11, -3, -5, -6, -5, 0, -4, -2, -7, 0, 0, -1, -11, -2, -11, -4, 2, -4, -10, 0, -3, -3,
                         -4, 0, -6, -6, 0, -2, 1, -4, -2, -3, -3, -3, -6, 0, -8, -4, -11, -6, -6, -10, -4, -6, -1, -2, -3, -8, -3, 4, -5, 0, -2, -21, -5,
                         -11, -3, -1, -2, -8, -8, 1, -5, -1, -1, -4, -4, -2, -5, -2, -5, -10, -4, -9, 0, -4, -4, 4, 3, -3, -3, -4, -2, 0, -11, -6, -9, -1
                       }
};

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

void clop_test(int argc, char** argv, int depth, int offset) {
    std::string dwarf_dwarfs(argv[4]), dwarf_control2(argv[6]), dwarf_control3(argv[8]), dwarf_control4(argv[10]),
            troll_dwarfs(argv[12]), troll_control2(argv[14]), troll_control3(argv[16]), troll_control4(argv[18]);
    std::string dwarf_conn(argv[20]), troll_conn(argv[22]);
    int d_d = std::stoi(dwarf_dwarfs), d_c2 = std::stoi(dwarf_control2), d_c3 = std::stoi(dwarf_control3), d_c4 = std::stoi(dwarf_control4);
    int t_d = std::stoi(troll_dwarfs), t_c2 = std::stoi(troll_control2), t_c3 = std::stoi(troll_control3), t_c4 = std::stoi(troll_control4);
    int d_conn = std::stoi(dwarf_conn), t_conn = std::stoi(troll_conn);

    EvalParameters new_troll = {t_d, 400, t_conn, 1, t_c2, t_c3, t_c4, false}, new_dwarf = {d_d, 400, d_conn, 1, d_c2, d_c3, d_c4, false};
    int result = quiet_selfplay(depth + offset, depth, new_dwarf, default_troll_eval) - quiet_selfplay(depth + offset, depth, default_dwarf_eval, new_troll);

    if (result == 0) {
        std::cout << "D" << std::endl;
    } else if (result > 0) {
        std::cout << "W" << std::endl;
    } else {
        std::cout << "L" << std::endl;
    }
}

void dwarf_clop(int argc, char** argv, int depth, int offset) {
    std::string dwarf_dwarfs(argv[4]), dwarf_control3(argv[6]), dwarf_control4(argv[8]), dwarf_conn(argv[10]);
    int d_d = std::stoi(dwarf_dwarfs), d_c3 = std::stoi(dwarf_control3), d_c4 = std::stoi(dwarf_control4);
    int d_conn = std::stoi(dwarf_conn);

    EvalParameters new_dwarf = {d_d, 400, d_conn, 1, 0, d_c3, d_c4, true};

    std::uniform_int_distribution<int> dist(0, 200);
    std::random_device rand;
    uint32_t random_index = dist(rand);
    int result = quiet_selfplay(depth + offset, depth, new_dwarf, default_troll_eval) - mocked[depth + offset][random_index];

    if (result == 0) {
        std::cout << "D" << std::endl;
    } else if (result > 0) {
        std::cout << "W" << std::endl;
    } else {
        std::cout << "L" << std::endl;
    }
}

void compute_clop_mock(int dwarf_depth, int troll_depth) {
    for (int i = 0; i < 14; i++) {
        std::cout << quiet_selfplay(dwarf_depth, troll_depth, default_dwarf_eval, default_troll_eval) << std::endl;
    }
}