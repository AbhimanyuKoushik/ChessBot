#pragma once

#include <array>
#include <bitboards.hpp>
#include <iostream>
#include <types.hpp>

constexpr Bitboard rook_occupancy_mask(Square sq) {
  Square occupancy_sq;
  int rank, file;
  Bitboard occupancy_mask = 0ULL;

  // Original code, this is indexed 1, basically of traditional ranks and files
  // are We can refactor make some modifications so that will be faster we are
  // doing subtraction for ranks and addition for files on top, and then
  // removing them by doing same thing in for loops, might as well not do both

  // Understandable code
  /*
  rank = (8 - (sq / 8));
  file = (1 + (sq % 8));

  for (int i = 2; i < rank; i++) {
    occupancy_sq = static_cast<Square>((8 - i) * 8 + (file - 1));
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = rank + 1; i < 8; i++) {
    occupancy_sq = static_cast<Square>((8 - i) * 8 + (file - 1));
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = 2; i < file; i++) {
    occupancy_sq = static_cast<Square>((8 - rank) * 8 + (i - 1));
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = file + 1; i < 8; i++) {
    occupancy_sq = static_cast<Square>((8 - rank) * 8 + (i - 1));
    set_bit(occupancy_mask, occupancy_sq);
  }
  */

  // Refactored code
  rank = (sq / 8);
  file = (sq % 8);

  for (int i = rank + 1; i < 7; i++) {
    occupancy_sq = static_cast<Square>(i * 8 + file);
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = 1; i < rank; i++) {
    occupancy_sq = static_cast<Square>(i * 8 + file);
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = 1; i < file; i++) {
    occupancy_sq = static_cast<Square>(rank * 8 + i);
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = file + 1; i < 7; i++) {
    occupancy_sq = static_cast<Square>(rank * 8 + i);
    set_bit(occupancy_mask, occupancy_sq);
  }

  return occupancy_mask;
}

constexpr Bitboard bishop_occupancy_mask(Square sq) {
  Square occupancy_sq;
  int rank, file;
  Bitboard occupancy_mask = 0ULL;

  // Similar to rook_occupancy_mask, the understandable code is indexed 1, we
  // will refactor it to remove unnecessary additions and subtractions

  // Understandable code
  /*
  rank = (8 - (sq / 8));
  file = (1 + (sq % 8));

  for (int i = 1; (rank - i > 1) && (file - i > 1); i++) {
    occupancy_sq = static_cast<Square>((8 - rank + i) * 8 + (file - i - 1));
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = 1; (rank + i < 8) && (file + i < 8); i++) {
    occupancy_sq = static_cast<Square>((8 - rank - i) * 8 + (file + i - 1));
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = 1; (rank - i > 1) && (file + i < 8); i++) {
    occupancy_sq = static_cast<Square>((8 - rank + i) * 8 + (file + i - 1));
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = 1; (rank + i < 8) && (file - i > 1); i++) {
    occupancy_sq = static_cast<Square>((8 - rank - i) * 8 + (file - i - 1));
    set_bit(occupancy_mask, occupancy_sq);
  }
  */

  // Refactored code
  rank = sq / 8;
  file = sq % 8;

  for (int i = 1; (rank - i > 0) && (file - i > 0); i++) {
    occupancy_sq = static_cast<Square>((rank - i) * 8 + (file - i));
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = 1; (rank + i < 7) && (file + i < 7); i++) {
    occupancy_sq = static_cast<Square>((rank + i) * 8 + (file + i));
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = 1; (rank - i > 0) && (file + i < 7); i++) {
    occupancy_sq = static_cast<Square>((rank - i) * 8 + (file + i));
    set_bit(occupancy_mask, occupancy_sq);
  }

  for (int i = 1; (rank + i < 7) && (file - i > 0); i++) {
    occupancy_sq = static_cast<Square>((rank + i) * 8 + (file - i));
    set_bit(occupancy_mask, occupancy_sq);
  }

  return occupancy_mask;
}