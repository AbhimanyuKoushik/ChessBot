#pragma once

#include <types.hpp>

// Sets a particular bit to 1
constexpr inline void set_bit(Bitboard& board, Square sq);

// Gets the value of a particular bit
constexpr inline bool get_bit(const Bitboard board, Square sq);

// Sets a particular value to 0
constexpr inline void pop_bit(Bitboard& board, Square sq);

// Gets the number of ones in a bitboard
constexpr inline uint8_t count_bits(const Bitboard board);

// Gets the least index among the indices of the bits whose value is 1
constexpr inline int get_lsb_index(const Bitboard board);

// prints the given bitboard
inline void print_bitboard(const Bitboard& board);

#include <details/bitboards_inline.inl>