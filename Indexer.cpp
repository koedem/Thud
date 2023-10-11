
#include "Indexer.h"

boost::multiprecision::uint128_t Indexer::index_from_dwarf_positions_without_piece_count(std::vector<int> &dwarves) {
    boost::multiprecision::uint128_t index = 0;
    int last_piece = -1;
    std::size_t pieces_left = dwarves.size();
    for (auto dwarf : dwarves) {
        index += access_stored_n_choose_k(163 - last_piece, pieces_left);
        index -= access_stored_n_choose_k(163 - dwarf + 1, pieces_left);

        last_piece = dwarf;
        pieces_left--;
    }
    return index;
}

uint64_t Indexer::index_from_troll_positions_without_piece_count(std::vector<int> &trolls) {
    uint64_t index = 0;
    int last_piece = -1;

    std::size_t pieces_left = trolls.size();
    for (auto troll : trolls) {
        index += (uint64_t) access_stored_n_choose_k(163 - last_piece, pieces_left);
        index -= (uint64_t) access_stored_n_choose_k(163 - troll + 1, pieces_left);

        last_piece = troll;
        pieces_left--;
    }
    return index;
}

boost::multiprecision::uint128_t Indexer::index_from_dwarf_positions(std::vector<int> &dwarves) {
    boost::multiprecision::uint128_t index = index_from_dwarf_positions_without_piece_count(dwarves);
    index *= 32;
    index += dwarves.size(); // encode the number of dwarves since otherwise there might be ambiguity
    return index;
}

uint64_t Indexer::index_from_troll_positions(std::vector<int> &trolls) {
    uint64_t index = index_from_troll_positions_without_piece_count(trolls);
    index *= 8;
    index += trolls.size(); // encode the number of trolls since otherwise there might be ambiguity
    return index;
}

/**
 * Calculates a tablebase index for a position. This index counts the number of possible smaller positions than the
 * given position, sorted by the sequence of piece locations ordered lexicographically.
 * @param pieces the position to be indexed
 * @return
 */
boost::multiprecision::uint128_t Indexer::index_dwarves(Board &board) {
    std::vector<int> dwarves;
    for (int id = 0; id < 164; id++) {
        if (board.get_square(index_to_square[id]) == Piece::DWARF) {
            dwarves.emplace_back(id);
        }
    }
    return index_from_dwarf_positions(dwarves);
}

uint64_t Indexer::index_trolls(Board &board) {
    std::vector<int> trolls;
    for (int id = 0; id < 164; id++) {
        if (board.get_square(index_to_square[id]) == Piece::TROLL) {
            trolls.emplace_back(id);
        }
    }
    return index_from_troll_positions(trolls);
}

template<Piece piece>
void Indexer::smallest_encoding_order(Board& board, std::vector<int>& pieces, bool smallest[8], int start_index, int most_recent_symmetry) {
    for (int symmetry = start_index; symmetry < 8; symmetry++) {
        if (!smallest[symmetry] || symmetry == most_recent_symmetry) {
            continue;
        }
        uint32_t piece_index = -1;
        bool improved = false;
        for (int id = 0; id < 164; id++) { // can we improve the baseline?
            if (board.get_square(symmetric_indices_to_squares[symmetry][id]) == piece) {
                piece_index++;
                if (!improved) {
                    if (pieces[piece_index] < id) {
                        smallest[symmetry] = false;
                        break;
                    } else if (pieces[piece_index] > id) { // our encoding is smaller confirmed
                        for (int small = 0; small < symmetry; small++) {
                            smallest[small] = false;
                        }
                        if (most_recent_symmetry >= 0) {
                            smallest[most_recent_symmetry] = false;
                        }
                        improved = true;
                    } else {
                        continue;
                    }
                }
                pieces[piece_index] = id;
            }
        }
    }
}

Indexer::Index Indexer::symmetric_index(Board& board, int most_recent_symmetry) {
    Index result{ -1, -1ull};

    std::vector<int> dwarves;
    for (int id = 0; id < 164; id++) { // first get a baseline of what the first encoding would look like
        if (board.get_square(symmetric_indices_to_squares[most_recent_symmetry][id]) == Piece::DWARF) {
            dwarves.emplace_back(id);
        }
    }

    bool smallest[8] = { true, true, true, true, true, true, true, true };
    smallest_encoding_order<Piece::DWARF>(board, dwarves, smallest, 0, most_recent_symmetry);

    result.dwarves = index_from_dwarf_positions(dwarves);

    std::vector<int> trolls;
    int symmetry = 0;
    for (; symmetry < 8; symmetry++) {
        if (!smallest[symmetry]) {
            continue;
        }

        result.symmetry = symmetry; // first candidate for dwarf symmetry
        for (int id = 0; id < 164; id++) { // first get a baseline of what the smallest encoding would look like
            if (board.get_square(symmetric_indices_to_squares[symmetry][id]) == Piece::TROLL) {
                trolls.emplace_back(id);
            }
        }
        break;
    }
    symmetry++;
    smallest_encoding_order<Piece::TROLL>(board, trolls, smallest, symmetry, -1);

    result.trolls = index_from_troll_positions(trolls);

    return result;
}