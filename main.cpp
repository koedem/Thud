#include <iostream>
#include <thread>
#include "Utils.h"
#include "MoveGenerator.h"
#include "Tablebase_test_128bit.h"
#include "Search.h"
#include "Perft.h"
#include "Timer.h"

MoveGenerator move_gen;

int most_captures = 0;

constexpr uint32_t num_bits = 27;

std::vector<Move> pv(128);
int pv_index = 0;

uint64_t perft(Board& board, int depth) {
    if (depth == 0) {
        int captures = board.number_of_captures();
        if (captures > most_captures) {
            most_captures = captures;
            for (int i = 0; i < pv_index; i++) {
                pv[i].print();
                std::cout << " ";
            }
            std::cout << std::endl;
        }
        most_captures = std::max(most_captures, board.number_of_captures());
        return 1;
    }
    uint64_t result = 0;
    std::vector<Move> moves;
    move_gen.generate_moves(moves, board);

    for (Move move : moves) {
        board.make_move(move);
        pv[pv_index++] = move;
        result += perft(board, depth - 1);
        board.unmake_move(move);
        pv[--pv_index] = Move();
    }
    return result;
}

void print_info(int depth, int eval, uint64_t nodes, uint64_t micros) {
    auto millis = micros / 1000;
    auto knps = nodes * 1000 / (micros + 1);
    std::cout << "info depth\t" << depth << "\tscore\t" << eval << "\tnodes\t" << nodes << "\tknps\t" << knps << "\ttime\t" << millis << "\tpv ";
}

void search_test(int depth_limit) {
    Board board(Position::Full);
    board.print();
    TranspositionTable tt(num_bits);
    Search search(board, tt);
    for (int depth = 1; depth <= depth_limit; depth++) {
        search.reset_nodes();
        Timer timer;
        int eval = search.pv_search(depth, MIN_EVAL, MAX_EVAL);
        print_info(depth, eval, search.get_nodes(), timer.elapsed());
        tt.print_pv(board, depth);
        tt.print_size();
    }
}

void perft_test(Board& board, int depth_limit) {
    board.print();
    most_captures = 0;
    for (int depth = 0; depth <= depth_limit; depth++) {
        std::cout << perft(board, depth) << " " << most_captures << std::endl;
    }
}

void game(int depth_limit) {
    Board board(Position::Full);
    board.print();
    TranspositionTable tt(num_bits);
    for (int i = 0; i < 300; i++) {
        Search search(board, tt);
        for (int depth = 1; depth <= depth_limit; depth++) {
            Timer timer;
            int eval = search.pv_search(depth, MIN_EVAL, MAX_EVAL);
            print_info(depth, eval, search.get_nodes(), timer.elapsed());
            tt.print_pv(board, depth);
            tt.print_size();
        }
        auto move = tt.at(board.get_index(), depth_limit).move;
        board.make_move(move);
        std::cout << std::endl << "New position" << std::endl << std::endl;
        board.print(move);
        std::cout << std::endl << std::endl;
        tt.clear();
    }
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

    for (int i = 0; i < 300; i++) {
        Search search(board, tt);
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

int main() {
    setup_valid_squares();
    //Tablebase_test test;
    //test.test_indexing(4);

    Board board(Position::Full);

    std::vector<Move> moves;
    move_gen.generate_moves(moves, board);

    //search_test(8);

    game(8, 4, 20000000, 0);

    //Perft_TT tt;
    //Perft perft(tt);

    //perft.root_perft(board, 7, true);
    /*for (Move move : moves) {
        board.make_move(move);
        board.print();

        int num_threads = 16;
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
    }*/


    //Board board(Position::Endgame);
    //perft_test(board, 6);

    //perft_test(board_2, 5);

    return 0;
}
