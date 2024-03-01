#pragma once

#include <iostream>
#include <random>

struct EvalParameters {
    int dwarf_factor, troll_factor;
    int dwarf_connection_factor;
    int center_factor;
    int control2, control3, control4;
    bool q_search;
};

constexpr int default_seed = 123456780;
static std::mt19937 seeder(default_seed);

constexpr EvalParameters default_dwarf_eval{100, 400, 117, 1, 0, 43, 50, true}, default_troll_eval{49, 400, 17, 1, 0, 41, 62, true};

constexpr EvalParameters dwarf_hce{20, 400, 80, 1, 0, 0, 0, true}, dwarf_clop_old{29, 400, 9, 1, 0, 27, 66, true},
        dwarf_ret{100, 400, 117, 1, 0, 43, 50, true}, dwarf_new_wide{321, 400, 45, 1, 0, 37, 45, true},
        dwarf_ret2_12k{290, 400, 63, 1, 0, 23, -5, true}, dwarf_ret2_narrow_21k{26, 400, 19, 1, 0, 15, 55, true}, dwarf_ret2_narrow_161k{16, 400, 14, 1, 0, 20, 19, true},
        dwarf_new_narrow_16k{63, 400, 12, 1, 0, 29, 46, true}, dwarf_new_narrow_35k{56, 400, 13, 1, 0, 32, 44, true}, dwarf_new_narrow_93k{52, 400, 14, 1, 0, 30, 46, true};
constexpr EvalParameters troll_hce{55, 400, 10, 1, 0, 0, 0, true}, troll_clop_old{8, 400, 20, 1, 0, 16, 133, true},
        troll_ret{49, 400, 17, 1, 0, 41, 62, true};

constexpr int assertion_level = 0;

// for main
constexpr uint32_t num_bits = 12;
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
constexpr uint32_t perft_tt_size = 1 << 0;
constexpr bool perft_store_to_file = true;
const std::string storage_path = "/media/kolja/Volume/StartPositionD7.tt";