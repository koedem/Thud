#include <vector>
#include <iostream>
#include <bit>
#include "Board.h"

template<>
void Board::fill_position<Position::Full>() {
    board[5] = board[6] = board[8] = board[9] = board[20] = board[26] = board[35] = board[43] = board[50] = board[60]
            = board[65] = board[77] = board[80] = board[94] = board[96] = board[110] = board[128] = board[142]
            = board[144] = board[158] = board[161] = board[173] = board[178] = board[188] = board[195] = board[203]
            = board[212] = board[218] = board[229] = board[230] = board[232] = board[233] = Piece::DWARF;
    board[102] = board[103] = board[104] = board[118] = board[120] = board[134] = board[135] = board[136] = Piece::TROLL;

    dwarfs_remaining = 32;
    trolls_remaining = 8;
    center_trolls = 8, center_dwarves = 0;
}

template<>
void Board::fill_position<Position::Endgame>() {
    board[233] = Piece::DWARF;
    board[102] = board[103] = board[104] = board[118] = board[120] = board[134] = board[135] = board[136] = Piece::TROLL;

    dwarfs_remaining = 1;
    trolls_remaining = 8;
    center_trolls = 8, center_dwarves = 0;
}

Board::Board(Position type) : board(255, Piece::NONE), to_move(Dwarf) {
    board[0] = board[1] = board[2] = board[3] = board[4] = board[10] = board[11] = board[12] = board[13] = board[14]
            = board[15] = board[16] = board[17] = board[18] = board[19] = board[27] = board[28] = board[29] = board[30]
            = board[31] = board[32] = board[33] = board[34] = board[44] = board[45] = board[46] = board[47] = board[48]
            = board[49] = board[61] = board[62] = board[63] = board[64] = board[78] = board[79] = board[95] = board[111]
            = board[127] = board[143] = board[159] = Piece::OUTSIDE;
    board[160] = board[174] = board[175] = board[176] = board[177] = board[189] = board[190] = board[191] = board[192]
            = board[193] = board[194] = board[204] = board[205] = board[206] = board[207] = board[208] = board[209]
            = board[210] = board[211] = board[219] = board[220] = board[221] = board[222] = board[223] = board[224]
            = board[225] = board[226] = board[227] = board[228] = board[234] = board[235] = board[236] = board[237]
            = board[238] = board[239] = board[240] = board[241] = board[242] = board[243] = board[244] = board[245]
            = board[246] = board[247] = board[248] = board[249] = board[250] = board[251] = board[252] = board[253]
            = board[254] = board[255] = Piece::OUTSIDE;
    board[119] = Piece::STONE;

    switch(type) {
        case Position::Full:        fill_position<Position::Full>();        break;
        case Position::Endgame:     fill_position<Position::Endgame>();     break;
    }
    attack_board.init_lines(*this);
    attack_board.init_empties(*this);
}

void Board::change_to_move() {
    to_move = flip_colour(to_move);
}

void Board::make_move(Move move) {
    if (move.captures != NO_CAPTURES) {
        if (move.to_move == Dwarf) {
            remove_troll(move.to); // Dwarfs capture a troll
        } else { // Troll captures all surrounding dwarves
            for (int dir = 0; dir < directions.size(); dir++) {
                if ((move.captures & (1 << dir)) != 0) {
                    remove_dwarf(move.to + directions[dir]);
                }
            }
        }
    }

    if (move.to_move == Dwarf) {
        remove_dwarf(move.from);
        add_dwarf(move.to);
    } else {
        remove_troll(move.from);
        add_troll(move.to);
    }
    change_to_move();
}

void Board::unmake_move(Move move) {
    change_to_move();
    if (move.to_move == Dwarf) {
        remove_dwarf(move.to);
        add_dwarf(move.from);
    } else {
        remove_troll(move.to);
        add_troll(move.from);
    }

    if (move.captures != NO_CAPTURES) {
        if (move.to_move == Dwarf) { // Dwarfs uncapture a troll
            add_troll(move.to);
        } else { // Troll uncaptures all surrounding dwarves
            for (int dir = 0; dir < directions.size(); dir++) {
                if ((move.captures & (1 << dir)) != 0) {
                    add_dwarf(move.to + directions[dir]);
                }
            }
        }
    }
}

Piece Board::get_square(Square square) const {
    return board[square];
}

Colour Board::get_to_move() const {
    return to_move;
}

void Board::print(Move move) const {
    for (Square i = 0; i < 16; i++) {
        for (Square j = 0; j < 16; j++) {
            Square square = i * 16 + j;

            if (move != NO_MOVE && (square == move.from || square == move.to)) {
                std::cout << "\033[1;7;33m";
            }

            Piece piece = board[square];
            if (piece == Piece::NONE) {
                std::cout << "0";
            } else if (piece == Piece::DWARF) {
                std::cout << "\033[32m";
                std::cout << "Z";
                std::cout << "\033[0m";
            } else if (piece == Piece::TROLL) {
                std::cout << "\033[36m";
                std::cout << "T";
                std::cout << "\033[0m";
            } else if (piece == Piece::STONE) {
                std::cout << "X";
            } else if (piece == Piece::OUTSIDE) {
                std::cout << "-";
            }

            if (move != NO_MOVE && (square == move.from || square == move.to)) {
                std::cout << "\033[0m";
            }
            std::cout << " ";
        }
        std::cout << std::endl;
    }
}

int Board::get_dwarf_count() const {
    return dwarfs_remaining;
}

int Board::get_troll_count() const {
    return trolls_remaining;
}

Indexer::Index Board::get_index() {
    return indexer.symmetric_index(*this);
}

int Board::get_dwarf_connections() const {
    return dwarf_connections;
}

int Board::get_dwarf_centers() const {
    return center_dwarves;
}

int Board::get_troll_centers() const {
    return center_trolls;
}

void Board::remove_dwarf(Square square) {
    board[square] = Piece::NONE;

    if (square_in_center(square)) {
        center_dwarves--;
    }

    for (int dir = 0; dir < directions.size(); dir++) {
        if (board[square + directions[dir]] == Piece::DWARF) {
            dwarf_connections--;
        }
    }
    dwarfs_remaining--;
    attack_board.remove_dwarf(*this, square);
}

void Board::add_dwarf(Square square) {
    board[square] = Piece::DWARF;

    if (square_in_center(square)) {
        center_dwarves++;
    }

    for (int dir = 0; dir < directions.size(); dir++) {
        if (board[square + directions[dir]] == Piece::DWARF) {
            dwarf_connections++;
        }
    }
    dwarfs_remaining++;
    attack_board.add_dwarf(*this, square);
}

void Board::remove_troll(Square square) {
    board[square] = Piece::NONE;
    if (square_in_center(square)) {
        center_trolls--;
    }

    trolls_remaining--;
    attack_board.remove_troll(*this, square);
}

void Board::add_troll(Square square) {
    board[square] = Piece::TROLL;
    if (square_in_center(square)) {
        center_trolls++;
    }

    trolls_remaining++;
    attack_board.add_troll(*this, square);
}

EvalType Board::get_eval(const EvalParameters& params) const {
    return evaluation.eval(*this, params);
}

const std::array<uint8_t, 8> &Board::empty_lengths(Square square) const {
    return attack_board.get_empty_spaces(square);
}

const std::array<uint8_t, 8>& Board::line_lengths(Square square) const {
    return attack_board.get_line_lengths(square);
}

const std::array<int, 16>& Board::get_controls() const {
    return attack_board.get_controls();
}
