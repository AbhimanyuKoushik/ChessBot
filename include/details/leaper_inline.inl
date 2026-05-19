#pragma once

#include <array>
#include <bitboards.hpp>
#include <types.hpp>

constexpr Bitboard mask_pawn_attacks(Color side, Square sq) {
  Bitboard attacks = 0ULL;
  Bitboard board = 0ULL;

  set_bit(board, sq);

  if (side == WHITE) {
    attacks |= (board >> 7 & kNotAFile);
    attacks |= (board >> 9 & kNotHFile);
  } else {
    attacks |= (board << 7 & kNotHFile);
    attacks |= (board << 9 & kNotAFile);
  }

  return attacks;
}

constexpr Bitboard mask_knight_attacks(Square sq) {
  Bitboard attacks = 0ULL;
  Bitboard board = 0ULL;

  set_bit(board, sq);

  attacks |= (board >> 17 & kNotHFile);
  attacks |= (board << 17 & kNotAFile);
  attacks |= (board >> 15 & kNotAFile);
  attacks |= (board << 15 & kNotHFile);
  attacks |= (board >> 10 & kNotGHFile);
  attacks |= (board << 10 & kNotABFile);
  attacks |= (board >> 6 & kNotABFile);
  attacks |= (board << 6 & kNotGHFile);

  return attacks;
}

constexpr Bitboard mask_king_attacks(Square sq) {
  Bitboard attacks = 0ULL;
  Bitboard board = 0ULL;

  set_bit(board, sq);

  attacks |= ((board >> 7) & kNotAFile);
  attacks |= ((board << 7) & kNotHFile);
  attacks |= (board >> 8);
  attacks |= (board << 8);
  attacks |= ((board >> 9) & kNotHFile);
  attacks |= ((board << 9) & kNotAFile);
  attacks |= ((board >> 1) & kNotHFile);
  attacks |= ((board << 1) & kNotAFile);

  return attacks;
}

constexpr std::array<std::array<Bitboard, 64>, 2> init_pawn_attacks() {
  Square sq;
  Bitboard white_pawn_attacks, black_pawn_attacks;
  std::array<std::array<Bitboard, 64>, 2> pawnAttacks{};

  for (uint8_t rank = 0; rank < 8; rank++) {
    for (uint8_t file = 0; file < 8; file++) {
      sq = static_cast<Square>(rank * 8 + file);
      white_pawn_attacks = mask_pawn_attacks(WHITE, sq);
      black_pawn_attacks = mask_pawn_attacks(BLACK, sq);
      pawnAttacks[WHITE][sq] = white_pawn_attacks;
      pawnAttacks[BLACK][sq] = black_pawn_attacks;
    }
  }

  return pawnAttacks;
}

constexpr std::array<Bitboard, 64> init_knight_attacks() {
  Square sq;
  std::array<Bitboard, 64> knightAttacks{};

  for (uint8_t rank = 0; rank < 8; rank++) {
    for (uint8_t file = 0; file < 8; file++) {
      sq = static_cast<Square>(rank * 8 + file);
      knightAttacks[sq] = mask_knight_attacks(sq);
    }
  }

  return knightAttacks;
}

constexpr std::array<Bitboard, 64> init_king_attacks() {
  Square sq;
  std::array<Bitboard, 64> kingAttacks{};

  for (uint8_t rank = 0; rank < 8; rank++) {
    for (uint8_t file = 0; file < 8; file++) {
      sq = static_cast<Square>(rank * 8 + file);
      kingAttacks[sq] = mask_king_attacks(sq);
    }
  }

  return kingAttacks;
}