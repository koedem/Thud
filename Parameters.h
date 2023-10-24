#pragma once

#include <iostream>

struct EvalParameters {
    int dwarf_factor, troll_factor;
    int dwarf_connection_factor;
};

// for main
constexpr uint32_t num_bits = 23;
constexpr uint32_t game_length = 200;
constexpr uint32_t num_threads = 16;

// for Search
constexpr bool use_extensions = false;
constexpr bool shuffled_pv_search = true;

static constexpr bool USE_TT = true;
static constexpr bool RAZORING = true;

// for Search TT
constexpr bool search_store_to_file = true;

// for Perft TT
constexpr int perft_tt_depth = 2;
constexpr uint32_t perft_tt_size = 1 << 27;
constexpr bool perft_store_to_file = true;
const std::string storage_path = "/media/kolja/Volume/StartPositionD7.tt";