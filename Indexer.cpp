
#include "Indexer.h"

/**
 * Calculates a tablebase index for a position. This index counts the number of possible smaller positions than the
 * given position, sorted by the sequence of piece locations ordered lexicographically.
 * @param pieces the position to be indexed
 * @return
 */
boost::multiprecision::uint128_t Indexer::index_dwarves(Board &board) {
    boost::multiprecision::uint128_t index = 0;
    int last_piece = -1;
    std::vector<int> dwarves;
    for (int id = 0; id < 164; id++) {
        if (board.get_square(index_to_square[id]) == Piece::DWARF) {
            dwarves.emplace_back(id);
        }
    }

    std::size_t pieces_left = dwarves.size();
    for (auto dwarf : dwarves) {
        index += access_stored_n_choose_k(163 - last_piece, pieces_left);
        index -= access_stored_n_choose_k(163 - dwarf + 1, pieces_left);

        last_piece = dwarf;
        pieces_left--;
    }
    return index;
}

uint64_t Indexer::index_trolls(Board &board) {
    uint64_t index = 0;
    int last_piece = -1;
    std::vector<int> trolls;
    for (int id = 0; id < 164; id++) {
        if (board.get_square(index_to_square[id]) == Piece::TROLL) {
            trolls.emplace_back(id);
        }
    }

    std::size_t pieces_left = trolls.size();
    for (auto troll : trolls) {
        index += (uint64_t) access_stored_n_choose_k(163 - last_piece, pieces_left);
        index -= (uint64_t) access_stored_n_choose_k(163 - troll + 1, pieces_left);

        last_piece = troll;
        pieces_left--;
    }
    return index;
}