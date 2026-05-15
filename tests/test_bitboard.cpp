#include <bitboards.hpp>
#include <iostream>
#include <types.hpp>

bool test_bitboard() {
  Bitboard board = 0ULL;
  set_bit(board, A8);
  if (board != ((1ULL << A8))) return false;
  set_bit(board, H1);
  if (board != ((1ULL << A8) | (1ULL << H1))) return false;
  set_bit(board, D4);
  if (board != ((1ULL << A8) | (1ULL << H1) | (1ULL << D4))) return false;
  print_bitboard(board);
  if (count_bits(board) != 3) return false;
  if (get_lsb_index(board) != A8) return false;
  return true;
}