#include "Utils.h"

bool valid_squares[256];

void setup_valid_squares() {
    for (int square = 0; square < 256; square++) {
        bool value = true;
        Square row = square / 16, file = square % 16;
        if (row >= 15 || file >= 15) { // out of range
            value = false;
        }
        if (row < 5 && 5 - row > file) { // top left
            value = false;
        }
        if (row < 5 && 5 - row > 14 - file) { // top right
            value = false;
        }
        if (row >= 9 && row - 9 > file) { // bottom left
            value = false;
        }
        if (row >= 9 && row - 9 > 14 - file) { // bottom right
            value = false;
        }
        if (row == 7 && file == 7) { // stone
            value = false;
        }
        valid_squares[square] = value;
    }
}

bool square_on_board(Square square) {
    return valid_squares[square];
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

boost::multiprecision::uint128_t n_choose_k(std::size_t n, std::size_t k) {
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
