#include <iostream>
#include "Utils.h"
#include "MoveGenerator.h"
#include "Tablebase_test_128bit.h"
#include "Search.h"
#include "Perft.h"

MoveGenerator move_gen;

int most_captures = 0;

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

void search_test(int depth_limit) {
    Board board(Position::Full);
    board.print();
    Search search;
    for (int depth = 1; depth <= depth_limit; depth++) {
        int eval = search.nega_max(board, depth, depth);
        std::cout << "Depth " << depth << ": " << eval << std::endl;
    }
}

void perft_test(Board& board, int depth_limit) {
    board.print();
    most_captures = 0;
    for (int depth = 0; depth <= depth_limit; depth++) {
        std::cout << perft(board, depth) << " " << most_captures << std::endl;
    }
}

int main() {
    //Tablebase_test test;
    //test.test_indexing(4);

    Board board(Position::Full);
    Perft perft;
    std::cout << (uint64_t) perft.hash_perft<Perft::NO_HASHING>(board, 2) << std::endl;
    //search_test(5);

    //Board board(Position::Endgame);
    //perft_test(board, 6);

    //perft_test(board_2, 5);

    return 0;
}
