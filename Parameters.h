#pragma once

#include <iostream>

struct EvalParameters {
    int dwarf_factor, troll_factor;
    int dwarf_connection_factor;
    int center_factor;
    int control2, control3, control4;
    bool q_search;
};

constexpr EvalParameters default_dwarf_eval{29, 400, 9, 1, 0, 27, 66, true}, default_troll_eval{55, 400, 10, 1, 0, 0, 0, true};

// for main
constexpr uint32_t num_bits = 10;
constexpr uint32_t game_length = 200;
constexpr uint32_t num_threads = 20;

// for Search
constexpr bool use_extensions = false;
constexpr bool shuffled_pv_search = true;

static constexpr bool USE_TT = true;
static constexpr bool RAZORING = false;

// for Search TT
constexpr bool search_store_to_file = true;

// for Perft TT
constexpr int perft_tt_depth = 2;
constexpr uint32_t perft_tt_size = 1 << 27;
constexpr bool perft_store_to_file = true;
const std::string storage_path = "/media/kolja/Volume/StartPositionD7.tt";