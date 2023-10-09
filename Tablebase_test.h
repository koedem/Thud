#pragma once

#include <cstdint>
#include <vector>
#include <cassert>
#include <iostream>

class Tablebase_test {

    /**
    * Cache for binomial coefficients, to be accessed as binoms[n][k] = n choose k
    */
    std::size_t binoms[165][10]{};

    static std::size_t n_choose_k(std::size_t n, std::size_t k) {
        if (k > n) {
            return 0;
        }
        if (k * 2 > n) {
            k = n - k;
        }
        if (k == 0) {
            return 1;
        }

        std::size_t result = n;
        for (std::size_t i = 2; i <= k; ++i) {
            result *= (n - i + 1);
            result /= i;
        }
        return result;
    }

    std::size_t access_stored_n_choose_k(std::size_t n, std::size_t k) {
        return binoms[n][k];
    }

    /**
     * Calculates a tablebase index for a position. This index counts the number of possible smaller positions than the
     * given position, sorted by the sequence of piece locations ordered lexicographically.
     * @param pieces the position to be indexed
     * @return
     */
    std::size_t index(std::vector<int> &pieces) {
        std::size_t index = 0;
        int last_piece = -1;
        std::size_t pieces_left = pieces.size();
        for (auto piece: pieces) {
            index += access_stored_n_choose_k(163 - last_piece, pieces_left);
            index -= access_stored_n_choose_k(163 - piece + 1, pieces_left);

            last_piece = piece;
            pieces_left--;
        }
        return index;
    }

    void generate_and_store_all_positions(std::vector<int> &pieces_so_far, std::size_t total_piece, std::vector<uint8_t> &tablebase,
                                          std::size_t piece_count_so_far = 0, int last_piece = -1) {
        if (piece_count_so_far == total_piece) {
            auto id = index(pieces_so_far);
            tablebase[id]++;
            return;
        }

        for (int i = last_piece + 1; i <= 164 - total_piece + piece_count_so_far; i++) {
            pieces_so_far[piece_count_so_far] = i;
            generate_and_store_all_positions(pieces_so_far, total_piece, tablebase, piece_count_so_far + 1, i);
        }
    }

    /**
     * Fills the cached binoms array with the correct values.
     */
    void prepare_binoms() {
        for (int i = 0; i < 165; i++) {
            for (int j = 0; j < 10; j++) {
                binoms[i][j] = n_choose_k(i, j);
            }
        }
    }

    /**
     * Testing function: iterate over all positions with a certain number of pieces and calculate the index for each
     * of them. Then verify that each index in the correct range was a calculation result exactly once.
     *
     * This currently assumes only one piece type.
     * @param num_pieces number of pieces of that one piece type.
     */
    void test_indexing(std::size_t num_pieces) {
        std::size_t size = n_choose_k(164, num_pieces);
        std::vector<uint8_t> tablebase(size);
        std::vector<int> pieces(num_pieces);
        generate_and_store_all_positions(pieces, num_pieces, tablebase);

        for (auto entry: tablebase) {
            if (entry != 1) {
                std::cout << "Indexing error" << std::endl;
                return;
            }
        }
    }

    Tablebase_test() {
        prepare_binoms();
    }
};