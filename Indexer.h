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

    void smallest_encoding_order(const Board& board, std::vector<int>& piece_locations, bool smallest[8]);

public:
    struct Index {
        [[no_unique_address]] boost::multiprecision::uint128_t piece_locations;
        uint32_t piece_order;
        uint8_t material;

        bool operator==(const Index& other) const
        {
            return (piece_locations == other.piece_locations && piece_order == other.piece_order && material == other.material);
        }
    };

    Indexer() {
        prepare_binoms();
        translate_squares();
        symmetry_translations();
    };

    Index index(const Board& board);

    Index symmetric_index(const Board& board);
};
