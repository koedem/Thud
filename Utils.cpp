#include "Utils.h"

bool square_on_board(Square square) {
    Square row = square / 16, file = square % 16;
    if (row >= 15 || file >= 15) { // out of range
        return false;
    }
    if (row < 5 && 5 - row > file) { // top left
        return false;
    }
    if (row < 5 && 5 - row > 14 - file) { // top right
        return false;
    }
    if (row >= 9 && row - 9 > file) { // bottom left
        return false;
    }
    if (row >= 9 && row - 9 > 14 - file) { // bottom right
        return false;
    }

    return true;
}

void print_colour(Colour colour) {
    if (colour == Dwarf) {
        std::cout << "Z";
    } else {
        std::cout << "T";
    }
}

void print_square(Square square) {
    int row = square / 16, file = square % 16;
    std::cout << char ('a' + file) << row;
}

Colour flip_colour(Colour c) {
    return static_cast<Colour>(!c);
}