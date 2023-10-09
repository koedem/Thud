#pragma once


#include <boost/multiprecision/cpp_int.hpp>
#include "Board.h"

class Indexer {

    /**
    * Cache for binomial coefficients, to be accessed as binoms[n][k] = n choose k
    */
    boost::multiprecision::uint128_t binoms[165][41]{};
    uint32_t index_to_square[164]{};

    static boost::multiprecision::uint128_t n_choose_k(std::size_t n, std::size_t k) {
        if (k > n) {
            return 0;
        }
        if (k * 2 > n) {
            k = n - k;
        }
        if (k == 0) {
            return 1;
        }

        boost::multiprecision::uint256_t result = n;
        for (std::size_t i = 2; i <= k; ++i) {
            result *= (n - i + 1);
            result /= i;
        }
        return static_cast<boost::multiprecision::uint128_t>(result);
    }

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

public:
    Indexer() {
        prepare_binoms();
        translate_squares();
    }

    boost::multiprecision::uint128_t index_dwarves(Board& board);

    uint64_t index_trolls(Board& board);
};
