#include <iostream>
#include <thread>
#include "Utils.h"
#include "MoveGenerator.h"
#include "Tablebase_test_128bit.h"
#include "Search.h"
#include "Perft.h"
#include "Timer.h"
#include "TranspositionTable.h"

MoveGenerator move_gen;

void print_info(int depth, int eval, uint64_t nodes, uint64_t micros) {
    auto millis = micros / 1000;
    auto knps = nodes * 1000 / (micros + 1);
    std::cout << "info depth\t" << depth << "\tscore\t" << eval << "\tnodes\t" << nodes << "\tknps\t" << knps << "\ttime\t" << millis << "\tpv ";
}

void search_test(int depth_limit) {
    Board board(Position::Full);
    board.print();
    TranspositionTable tt(num_bits);
    Search search(board, tt, default_eval);
    for (int depth = 1; depth <= depth_limit; depth++) {
        search.reset_nodes();
        Timer timer;
        int eval = search.pv_search(depth, MIN_EVAL, MAX_EVAL);
        print_info(depth, eval, search.get_nodes(), timer.elapsed());
        tt.print_pv(board, depth);
        tt.print_size();
    }
}

int quiet_selfplay(int dwarf_depth, int troll_depth) {
    Board board(Position::Full);
    TranspositionTable tt(num_bits);
    int final_eval;

    auto loop_condition = [&](int depth, uint64_t time) {
        if (board.get_to_move() == Dwarf) {
            return depth <= dwarf_depth;
        } else {
            return depth <= troll_depth;
        }
    };

    for (int i = 0; i < game_length; i++) {
        Search search(board, tt, default_eval);
        Timer timer;
        int depth = 1;
        for (; loop_condition(depth, timer.elapsed()); depth++) {
            timer.reset();
            final_eval = search.pv_search(depth, MIN_EVAL, MAX_EVAL);
        }
        auto move = tt.at(board.get_index(), depth - 1).move;
        board.make_move(move);
        //move.print();
        //std::cout << " " << std::flush;
        tt.clear();
    }
    //board.print();
    return board.get_dwarf_count() - board.get_troll_count() * 4;
}

void game(int dwarf_depth, int troll_depth, uint64_t dwarf_time_micros, uint64_t troll_time_micros) {
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
        Search search(board, tt, default_eval);
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

void game(int depth_limit) {
    game(depth_limit, depth_limit, 0, 0);
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

void human_vs_bot(int time_limit, Colour human_colour) {
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
        Search search(board, tt, default_eval);
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

int main() {
    setup_valid_squares();
    //Tablebase_test test;
    //test.test_indexing(4);
    search_test(8);
    // parallel_perft();
    int dwarf_depth = 6, troll_depth = 4;
    std::cout << "Dwarf depth: " << dwarf_depth << ", troll depth: " << troll_depth << std::endl;
    for (int i = 0; i < 125; i++) {
        std::cout << quiet_selfplay(dwarf_depth, troll_depth) << std::endl;
    }
    //game(5);
    //human_vs_bot(1000000, Troll);

    //game(8, 5, 20000000, 50000);
    return 0;
}
