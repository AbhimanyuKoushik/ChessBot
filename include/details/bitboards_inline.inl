#pragma once

#include <bit>
#include <bitboards.hpp>
#include <iostream>

// Sets a particular bit to 1
constexpr inline void set_bit(Bitboard& board, Square sq) {
  board |= (1ULL << sq);
}

// Gets the value of a particular bit
constexpr inline bool get_bit(const Bitboard board, Square sq) {
  return board & (1ULL << sq);
}

// Sets a particular value to 0
constexpr inline void pop_bit(Bitboard& board, Square sq) {
  board &= ~(1ULL << sq);
}

// Gets the number of ones in a bitboard
constexpr inline uint8_t count_bits(const Bitboard board) {
  return std::popcount(board);
}

// Gets the least index among the indices of the bits whose value is 1
constexpr inline int get_lsb_index(const Bitboard board) {
  return board ? std::countr_zero(board) : -1;
}

// prints the given bitboard
inline void print_bitboard(const Bitboard& board) {
  std::cout << '\n';
  Square sq;
  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      sq = static_cast<Square>(rank * 8 + file);
      if (file == 0) std::cout << 8 - rank << ": ";
      std::cout << (get_bit(board, sq) ? " 1" : " 0");
    }
    std::cout << '\n';
  }
  std::cout << '\n' << "    a b c d e f g h" << '\n';
  std::cout << "Bitboard: " << board << "\n\n";
}

// k infront tells that its a global constant, no change needed
inline constexpr Bitboard kNotAFile = 0xFEFEFEFEFEFEFEFEULL;
inline constexpr Bitboard kNotHFile = 0x7F7F7F7F7F7F7F7FULL;
inline constexpr Bitboard kNotGHFile = 0x3F3F3F3F3F3F3F3FULL;
inline constexpr Bitboard kNotABFile = 0xFCFCFCFCFCFCFCFCULL;