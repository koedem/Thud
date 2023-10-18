
#include "Indexer.h"
#include "Board.h"

template<class Value>
Value Indexer::index_from_piece_positions_without_piece_count(std::vector<int> &pieces, int n_choose) {
    Value index = 0;
    int last_piece = -1;

    std::size_t pieces_left = pieces.size();
    for (auto piece : pieces) {
        index += (Value) access_stored_n_choose_k(n_choose - 1 - last_piece, pieces_left);
        index -= (Value) access_stored_n_choose_k(n_choose - piece, pieces_left);

        last_piece = piece;
        pieces_left--;
    }
    return index;
}

Indexer::SmallIndex Indexer::small_index(Board& board) {
    std::vector<int> piece_locations;
    std::vector<int> troll_locations_within_piece_locations;
    for (int id = 0; id < 164; id++) {
        auto piece = board.get_square(index_to_square[id]);
        if (piece != Piece::NONE) {
            if (piece == Piece::TROLL) {
                troll_locations_within_piece_locations.emplace_back(piece_locations.size());
            }
            piece_locations.emplace_back(id);
        }
    }

    auto piece_index = index_from_piece_positions_without_piece_count<boost::multiprecision::uint128_t> (piece_locations);
    auto troll_index = index_from_piece_positions_without_piece_count<uint32_t> (troll_locations_within_piece_locations, piece_locations.size());
    uint8_t material = (piece_locations.size() - troll_locations_within_piece_locations.size() - 1) * 8 + (troll_locations_within_piece_locations.size() - 1);
    return {piece_index, troll_index, material};
}

void Indexer::small_smallest_encoding_order(Board& board, std::vector<int>& piece_locations, bool smallest[8]) {
    int most_recent_symmetry = this->most_recent_symmetry;
    for (int symmetry = 0; symmetry < 8; symmetry++) {
        if (!smallest[symmetry] || symmetry == most_recent_symmetry) {
            continue;
        }
        uint32_t piece_index = -1;
        bool improved = false;
        for (int id = 0; id < 164; id++) { // can we improve the baseline?
            if (board.get_square(symmetric_indices_to_squares[symmetry][id]) != Piece::NONE) {
                piece_index++;
                if (!improved) {
                    if (piece_locations[piece_index] < id) {
                        smallest[symmetry] = false;
                        break;
                    } else if (piece_locations[piece_index] > id) { // our encoding is smaller confirmed
                        for (int small = 0; small < symmetry; small++) {
                            smallest[small] = false;
                        }
                        smallest[most_recent_symmetry] = false;
                        this->most_recent_symmetry = symmetry;

                        improved = true;
                    } else {
                        continue;
                    }
                }
                piece_locations[piece_index] = id;
            }
        }
    }
}

Indexer::SmallIndex Indexer::symmetric_small_index(Board& board) {
    SmallIndex result;

    int most_recent_symmetry = this->most_recent_symmetry;
    std::vector<int> piece_locations;
    for (int id = 0; id < 164; id++) { // first get a baseline of what the first encoding would look like
        auto piece = board.get_square(symmetric_indices_to_squares[most_recent_symmetry][id]);
        if (piece != Piece::NONE) {
            piece_locations.emplace_back(id);
        }
    }

    bool smallest[8] = { true, true, true, true, true, true, true, true };
    small_smallest_encoding_order(board, piece_locations, smallest);

    result.dwarves = index_from_piece_positions_without_piece_count<boost::multiprecision::uint128_t> (piece_locations);

    std::vector<int> troll_locations_within_piece_locations;
    most_recent_symmetry = this->most_recent_symmetry;
    for (int id = 0; id < piece_locations.size(); id++) { // first get a baseline of what the smallest encoding would look like
        auto square = piece_locations[id];
        if (board.get_square(symmetric_indices_to_squares[most_recent_symmetry][square]) == Piece::TROLL) {
            troll_locations_within_piece_locations.emplace_back(id);
        }
    }

    for (int symmetry = 0; symmetry < 8; symmetry++) {
        if (!smallest[symmetry] || symmetry == most_recent_symmetry) {
            continue;
        }
        uint32_t piece_index = -1;
        bool improved = false;
        for (int id = 0; id < piece_locations.size(); id++) { // first get a baseline of what the smallest encoding would look like
            auto square = piece_locations[id];
            if (board.get_square(symmetric_indices_to_squares[symmetry][square]) == Piece::TROLL) {
                piece_index++;
                if (!improved) {
                    if (troll_locations_within_piece_locations[piece_index] < id) {
                        smallest[symmetry] = false;
                        break;
                    } else if (troll_locations_within_piece_locations[piece_index] > id) {
                        for (int small = 0; small < symmetry; small++) {
                            smallest[small] = false;
                        }
                        smallest[most_recent_symmetry] = false;
                        this->most_recent_symmetry = symmetry;

                        improved = true;
                    } else {
                        continue;
                    }
                }
                troll_locations_within_piece_locations[piece_index] = id;
            }
        }
    }

    result.trolls = index_from_piece_positions_without_piece_count<uint32_t> (troll_locations_within_piece_locations, piece_locations.size());
    uint8_t material = (piece_locations.size() - troll_locations_within_piece_locations.size() - 1) * 8 + (troll_locations_within_piece_locations.size() - 1);
    result.material = material;

    return result;
}

void Indexer::prepare_binoms() {
    for (int i = 0; i < 165; i++) {
        for (int j = 0; j < 41; j++) {
            binoms[i][j] = n_choose_k(i, j);
        }
    }
}

void Indexer::translate_squares() {
    int i = 0;
    for (int square = 0; square < 256; square++) {
        if (square_on_board(square)) {
            index_to_square[i] = square;
            i++;
        }
    }
}

int Indexer::square_from_file_row(int file, int row, int symmetry) {
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

void Indexer::symmetry_translations() {
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
