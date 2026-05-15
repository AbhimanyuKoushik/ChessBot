#pragma once

#include <bitboards.hpp>
#include <iostream>
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

  attacks |= (board >> 7 & kNotAFile);
  attacks |= (board << 7 & kNotHFile);
  attacks |= (board >> 8);
  attacks |= (board << 8);
  attacks |= (board >> 9 & kNotHFile);
  attacks |= (board << 9 & kNotAFile);
  attacks |= (board >> 1 & kNotHFile);
  attacks |= (board << 1 & kNotAFile);

  return attacks;
}