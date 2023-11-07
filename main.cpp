#include <iostream>
#include <thread>
#include "Utils.h"
#include "MoveGenerator.h"
#include "Tablebase_test_128bit.h"
#include "Search.h"
#include "Perft.h"
#include "Timer.h"
#include "TranspositionTable.h"
#include "clop.h"

MoveGenerator move_gen;

void print_info(int depth, int eval, uint64_t nodes, uint64_t micros) {
    auto millis = micros / 1000;
    auto knps = nodes * 1000 / (micros + 1);
    std::cout << "info depth\t" << depth << "\tscore\t" << eval << "\tnodes\t" << nodes << "\tknps\t" << knps << "\ttime\t" << millis << "\tpv ";
}

void search_test(int depth_limit, EvalParameters eval_params) {
    Board board(Position::Full);
    board.print();
    TranspositionTable tt(num_bits);
    Search search(board, tt, eval_params);
    for (int depth = 1; depth <= depth_limit; depth++) {
        search.reset_nodes();
        Timer timer;
        int eval = search.pv_search(depth, MIN_EVAL, MAX_EVAL);
        print_info(depth, eval, search.get_nodes(), timer.elapsed());
        tt.print_pv(board, depth);
        tt.print_size();
    }
}

void game(int dwarf_depth, int troll_depth, uint64_t dwarf_time_micros, uint64_t troll_time_micros,
          EvalParameters dwarf_eval, EvalParameters troll_eval) {
    Board board(Position::Full);
    board.print();
    TranspositionTable tt(num_bits);

    auto loop_condition = [&](int depth, uint64_t time) {
        if (board.get_to_move() == Dwarf) {
            return depth <= dwarf_depth || time <= dwarf_time_micros;
        } else {
            return depth <= troll_depth || time <= troll_time_micros;
        }
    };

    for (int i = 0; i < game_length; i++) {
        Search search(board, tt,  board.get_to_move() == Dwarf ? dwarf_eval : troll_eval);
        Timer timer;
        int depth = 1;
        for (; loop_condition(depth, timer.elapsed()); depth++) {
            timer.reset();
            int eval = search.pv_search(depth, MIN_EVAL, MAX_EVAL);
            print_info(depth, eval, search.get_nodes(), timer.elapsed());
            tt.print_pv(board, depth);
            tt.print_size();
        }
        auto move = tt.at(board.get_index(), depth - 1).move;
        board.make_move(move);
        std::cout << std::endl << "New position" << std::endl << std::endl;
        board.print(move);
        std::cout << std::endl << std::endl;
        tt.clear();
    }
}

void game(int depth_limit, EvalParameters dwarf_eval, EvalParameters troll_eval) {
    game(depth_limit, depth_limit, 0, 0, dwarf_eval, troll_eval);
}

void parallel_perft() {
    Board board(Position::Full);

    std::vector<Move> moves;
    move_gen.generate_moves(moves, board);

    Perft_TT tt;
    for (Move move : moves) {
        board.make_move(move);
        board.print();
        std::vector<Perft> perfts;
        perfts.reserve(num_threads);
        for (int i = 0; i < num_threads; i++) {
            perfts.emplace_back(tt);
        }

        std::vector<std::thread> search_threads;
        for (size_t i = 0; i < num_threads; i++) {
            auto func = std::bind(&Perft::root_perft, perfts[i], board, 6, i == 0);
            search_threads.emplace_back(func);
        }
        for (auto &thread: search_threads) {
            thread.join();
        }
        tt.store();
        board.unmake_move(move);
    }
}

Move parse_move(const std::string& move_str, Board& board) {
    std::vector<Move> moves;
    move_gen.generate_moves(moves, board);
    for (Move move : moves) {
        if (to_string(move) == move_str) {
            return move;
        }
    }
    return NO_MOVE;
}

Move get_human_move(Board& board) {
    Move human = NO_MOVE;
    while (human == NO_MOVE) {
        std::string human_move;
        std::cin >> human_move;
        human = parse_move(human_move, board);
    }
    return human;
}

void human_vs_bot(int time_limit, Colour human_colour, EvalParameters eval_params = default_dwarf_eval) {
    Board board(Position::Full);
    board.print();
    TranspositionTable tt(num_bits);

    if (human_colour == Dwarf) {
        Move human = get_human_move(board);
        board.make_move(human);
        std::cout << std::endl << "New position" << std::endl << std::endl;
        board.print(human);
        std::cout << std::endl << std::endl;
    }

    for (int i = 0; i < game_length; i++) {
        Search search(board, tt, eval_params);
        Timer timer;
        int depth = 1;
        for (; depth <= 4 || (timer.elapsed() < time_limit && depth < 50); depth++) {
            timer.reset();
            int eval = search.pv_search(depth, MIN_EVAL, MAX_EVAL);
            print_info(depth, eval, search.get_nodes(), timer.elapsed());
            tt.print_pv(board, depth);
            tt.print_size();
        }
        auto move = tt.at(board.get_index(), depth - 1).move;
        board.make_move(move);
        std::cout << std::endl << "New position" << std::endl << std::endl;
        board.print(move);
        std::cout << std::endl << std::endl;
        tt.clear();

        Move human = get_human_move(board);
        board.make_move(human);
        std::cout << std::endl << "New position" << std::endl << std::endl;
        board.print(human);
        std::cout << std::endl << std::endl;
    }
}

void eval_tuning(int depth, int repeats, int slices, int slice) {
    std::cout << "Depth: " << depth << ", slice: " << slice << " / " << slices  << std::endl;

    int slice_size = 48 / slices;
    for (int troll_eval = slice_size * slice; troll_eval < slice_size * (slice + 1); troll_eval++) {
        int troll_connection = troll_eval % 4, troll_troll_value = 16 + (troll_eval / 4);
        for (int dwarf_eval = 0; dwarf_eval < 76; dwarf_eval++) {
            int connection = (dwarf_eval % 4) + 1, troll_value = 9 + (dwarf_eval / 4);

            int result = 0;
            EvalParameters dwarf_side = {4, troll_value, connection}, troll_side = {4, troll_troll_value, troll_connection};
            for (int repeat = 0; repeat < repeats; repeat++) {
                result += quiet_selfplay(depth, depth, dwarf_side, troll_side);
            }
            std::cout << result / repeats << "\t" << std::flush;
        }
        std::cout << std::endl;
    }
}

int main(int argc, char** argv) {
    setup_valid_squares();
    setup_center_squares();
    //Tablebase_test test;
    //test.test_indexing(4);
    search_test(8, default_dwarf_eval);
    // parallel_perft();
    //game(6, 5, 2000000, 50000, default_eval, {1, 4, 1});
    //game(5);
    //compute_clop_mock(5, 3); /*
    //dwarf_clop(argc, argv, 3, 1); /*
    //clop_test(argc, argv, 3, 0);/*
    int dwarf_depth = 4, troll_depth = 3, repeats = 1;
    //eval_tuning(dwarf_depth, repeats, 12, 11);
    EvalParameters new_dwarf = {50, 400, 0, 1, 20, 40, 100}, new_troll = {55, 400, 10, 1};
    //game(4, 3, 0, 0, new_dwarf, default_troll_eval);/*
    for (int i = 0; i < 100; i++) { // 25 minutes
        int dwarf_result = quiet_selfplay(dwarf_depth, troll_depth, new_dwarf, default_troll_eval);
        int troll_result = quiet_selfplay(dwarf_depth, troll_depth, default_troll_eval, new_troll);
        std::cout << dwarf_result - troll_result << " " << dwarf_result << " " << troll_result << std::endl;
    }
    //human_vs_bot(1000000, Troll);*/

    return 0;
}
