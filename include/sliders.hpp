#pragma once

#include <array>
#include <bitboards.hpp>
#include <types.hpp>

// #########################################
// Occupancy Mask generation
// #########################################

constexpr Bitboard rook_occupancy_mask(Square sq);
constexpr Bitboard bishop_occupancy_mask(Square sq);

// storing the occupancy mask in a lookup table so that its fast later on
constexpr std::array<Bitboard, 64> store_rook_occupancy_masks();
constexpr std::array<Bitboard, 64> store_bishop_occupancy_masks();

#include <details/sliders_inline.inl>

// #########################################
// For loop Attack Mask generation
// #########################################

constexpr Bitboard rook_attack_gen(Bitboard occupied_squares, Square sq);
constexpr Bitboard bishop_attack_gen(Bitboard occupied_squares, Square sq);

// ###############################################
// Magic Numbers and initializing the hash table
// ###############################################

constexpr std::array<std::array<Bitboard, 4096>, 64>
initialize_rook_magic_luts();

constexpr std::array<std::array<Bitboard, 512>, 64>
initialize_bishop_magic_luts();

// #########################################
// For getting the final attack mask
// #########################################

inline Bitboard RookAttacks(Bitboard full_occupancies, Square sq);
inline Bitboard BishopAttacks(Bitboard full_occupancies, Square sq);
inline Bitboard QueenAttacks(Bitboard full_occupancies, Square sq);