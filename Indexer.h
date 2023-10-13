#pragma once


#include <boost/multiprecision/cpp_int.hpp>
#include "Board.h"

class Indexer {

    /**
    * Cache for binomial coefficients, to be accessed as binoms[n][k] = n choose k
    */
    boost::multiprecision::uint128_t binoms[165][41]{};
    uint32_t index_to_square[164]{};
    uint32_t symmetric_indices_to_squares[8][164]{};

    int most_recent_symmetry = 0;

    boost::multiprecision::uint128_t access_stored_n_choose_k(std::size_t n, std::size_t k) {
        return binoms[n][k];
    }

    /**
     * Fills the cached binoms array with the correct values.
     */
    void prepare_binoms() {
        for (int i = 0; i < 165; i++) {
            for (int j = 0; j < 41; j++) {
                binoms[i][j] = n_choose_k(i, j);
            }
        }
    }

    void translate_squares() {
        int i = 0;
        for (int square = 0; square < 256; square++) {
            if (square_on_board(square)) {
                index_to_square[i] = square;
                i++;
            }
        }
    }

    static int square_from_file_row(int file, int row, int symmetry) {
        if ((symmetry & 4) != 0) {
            file = 15 - file;
        }
        if ((symmetry & 2) != 0) {
            row = 15 - row;
        }

        if (symmetry % 2 == 1) {
            std::swap(row, file);
        }
        return 16 * row + file;
    }

    void symmetry_translations() {
        for (int symmetry = 0; symmetry < 8; symmetry++) {
            int i = 0;
            for (int row = 0; row < 16; row++) {
                for (int file = 0; file < 16; file++) {
                    int square = square_from_file_row(file, row, symmetry);
                    if (square_on_board(square)) {
                        symmetric_indices_to_squares[symmetry][i] = square;
                        i++;
                    }
                }
            }
        }
    }

    boost::multiprecision::uint128_t index_from_dwarf_positions_without_piece_count(std::vector<int>& dwarves);

    uint64_t index_from_troll_positions_without_piece_count(std::vector<int>& trolls);

    boost::multiprecision::uint128_t index_from_dwarf_positions(std::vector<int>& dwarves);

    uint64_t index_from_troll_positions(std::vector<int>& trolls);

    template<Piece piece>
    void smallest_encoding_order(Board& board, std::vector<int>& pieces, bool smallest[8]);

public:
    struct Index {
        boost::multiprecision::uint128_t dwarves;
        uint64_t trolls;
    };

    Indexer() {
        prepare_binoms();
        translate_squares();
        symmetry_translations();
    }

    boost::multiprecision::uint128_t index_dwarves(Board& board);

    uint64_t index_trolls(Board& board);

    Index symmetric_index(Board &board);
};
