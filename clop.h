#pragma once

#include <random>

int mocked[6][200] = { {}, {}, {}, { -31, -31, -31, -31, -23, -27, -15, -27, -23, -11, -27, -31, -31, -6, -31, -31, -27, -31, -31, -31, -31, -21, -31, -27, -8, -23, -31, -31,
                    -27, -31, -27, -31, -31, -27, -31, -27, -23, -27, -19, -31, -31, -31, -27, -22, -31, -27, -7, -31, -31, -31, -19, -23, -23, -27, -27, -27, -27, -31, -3,
                    -23, -31, -27, -31, -23, -31, -23, -31, -27, -31, -31, -31, -27, -31, -32, -27, -23, -27, -31, -23, -19, -31, -31, -31, -27, -31, -4, -27, -31, -31, -31,
                    -31, -31, -31, -23, -27, -31, -19, -27, -31, -31, -27, -31, -27, -23, -31, -27, -31, -31, -31, -31, -31, -31, -27, -31, -6, -27, -27, -23, -31, -31, -3,
                    -27, -31, -23, -23, -27, -31, -31, -31, -23, -22, -27, -31, -31, -31, -31, -31, -19, -27, -31, -27, -31, -31, -27, -15, -31, -15, -31, -27, -31, -27, -27,
                    -27, -27, -31, 1, -31, -31, -31, -31, -31, -27, -31, -28, -31, -27, -19, -31, -27, -31, -32, -31, -27, -8, -31, -31, -22, -31, -31, -27, -23, -27, -15,
                    -31, -31, -31, -31, -31, -23, -31, -27, -31, -31, -31, -31, -31, -23, -31, -27, -31 },
                       { -23, -5, -22, -22, -9, -10, -10, -7, -6, -4, 1, -11, -24, -14, -7, -22, -12, -26, -18, -22, -26, -22, -22, -22, -13, -17, -10, -8, -10, -30, -22, -8,
                    -12, -11, -5, -15, -26, -11, -19, -6, -11, -13, -30, -9, -8, -15, -26, -10, -6, -11, -22, -14, -22, -13, -2, -7, -18, -26, -13, -26, -12, -30, -5, -13,
                    -12, -12, -22, -17, -9, -18, -13, -6, -12, -15, -12, -13, -18, -9, -8, -14, -9, -3, -20, -17, -14, -3, -17, -12, -26, -26, -8, -11, -18, -26, -12, -9,
                    -12, -4, -22, -14, -26, -14, -9, -18, -24, -12, -30, -26, -12, -8, -12, -15, 6, -11, -10, -13, -11, -11, 0, -22, -8, -13, -22, -21, -3, -17, -7, -10,
                    -26, -4, -11, -22, -7, -14, -15, -26, -21, -1, -30, -10, -12, -18, -14, -30, -22, -14, -11, -11, -30, -9, 0, -4, -14, -11, -6, -13, 2, -26, -26, -5, -17,
                    -17, -7, -10, -22, -5, -16, -10, -13, -9, -14, -10, -7, -17, -12, -10, -22, -17, -26, -13, -10, -6, -10, -12, -7, -10, -15, -22, -26, -8, -6, -14, -2,
                    -13, -8, -22, -26, -20, -19, -12 },
                       { -26, -29, -30, -19, -30, -18, -9, -30, -8, -7, -29, -26, -26, -11, -24, -18, -30, -26, 3, -30, -26, -7, -30, -30, -16, -6, -18, -30, -8,
                    -30, -3, -12, -22, -6, -18, -22, -18, -14, -12, -30, -30, -13, -22, -9, -15, -30, -18, -26, -21, -9, -6, -10, -22, -16, -3, -9,
                    -12, -26, -17, -2, -22, -6, -15, -8, -30, -11, -26, -30, -4, -30, -10, -13, -4, -2, -11, -2, -9, -30, -13, 6, -6, -10, -30, -13,
                    -13, -26, -10, -15, 3, -17, -18, 7, -26, -10, -30, -2, -15, -31, -30, -30, -21, -26, -11, 13, -30, -30, -7, -16, -9, -5, -26, -10, -14, -16, -9,
                    -5, -6, -20, -10, -12, -30, -9, -26, -26, -26, -22, -7, -11, -26, -7, -20, -11, -10, -22, -30, -30, -21, 3, -30, -9, -24, -26, -8, -5, -1, -20,
                    -18, -26, -9, -12, -9, -20, 0, -15, -7, -18, -12, -30, 3, -7, -26, -8, -14, -2, -12, -13, -8, -9, -5, -15, -19, -4, 3, -22, -15, 5,
                    -6, -30, -8, -11, -14, -30, -22, 1, -30, -1, -10, 9, 6, -12, 3, -30, -18, -11, -18, -26, -13, -13, -26, -13   }
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

    EvalParameters new_troll = {t_d, 400, t_conn, 1, t_c2, t_c3, t_c4}, new_dwarf = {d_d, 400, d_conn, 1, d_c2, d_c3, d_c4};
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

    EvalParameters new_dwarf = {d_d, 400, d_conn, 1, 0, d_c3, d_c4};

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
    for (int i = 0; i < 200; i++) {
        std::cout << quiet_selfplay(dwarf_depth, troll_depth, default_dwarf_eval, default_troll_eval) << ", " << std::flush;
    }
}