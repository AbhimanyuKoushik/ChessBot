#pragma once

#include <array>
#include <bitboards.hpp>
#include <types.hpp>

constexpr Bitboard mask_pawn_attacks(Color side, Square sq);
constexpr Bitboard mask_knight_attacks(Square sq);
constexpr Bitboard mask_king_attacks(Square sq);

constexpr std::array<std::array<Bitboard, 64>, 2> init_pawn_attacks();
constexpr std::array<Bitboard, 64> init_knight_attacks();
constexpr std::array<Bitboard, 64> init_king_attacks();

#include <details/leaper_inline.inl>

inline constexpr std::array<std::array<Bitboard, 64>, 2> kPawnAttacks =
    init_pawn_attacks();
inline constexpr std::array<Bitboard, 64> kKnightAttacks =
    init_knight_attacks();
inline constexpr std::array<Bitboard, 64> kKingAttacks = init_king_attacks();
