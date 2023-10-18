#pragma once

#include <boost/multiprecision/cpp_int.hpp>
class Board;

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
    void prepare_binoms();

    void translate_squares();

    static int square_from_file_row(int file, int row, int symmetry);

    void symmetry_translations();

    template<class Value>
    Value index_from_piece_positions_without_piece_count(std::vector<int>& pieces, int n_choose = 164);

    void small_smallest_encoding_order(Board& board, std::vector<int>& piece_locations, bool smallest[8]);

public:
    struct SmallIndex {
        [[no_unique_address]] boost::multiprecision::uint128_t dwarves; // TODO piece_locations;
        uint32_t trolls; // TODO piece_order;
        uint8_t material;

        bool operator==(const Indexer::SmallIndex& other) const
        {
            return (dwarves == other.dwarves && trolls == other.trolls && material == other.material);
        }
    };

    Indexer() {
        prepare_binoms();
        translate_squares();
        symmetry_translations();
    };

    SmallIndex small_index(Board& board);

    SmallIndex symmetric_small_index(Board& board);
};
