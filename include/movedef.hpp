#pragma once

#include <cstdint>
#include <types.hpp>

using Move = uint16_t;

/*
Every move is encoded with a 16bit number -
The first 6 bits represent the source square
The next 6 bits represent the target square
The last 4 are for move flags
*/

enum MoveFlag : uint8_t {
  QUIET = 0,
  DOUBLE_PUSH = 1,
  KING_CASTLE = 2,
  QUEEN_CASTLE = 3,
  CAPTURE = 4,
  ENPASS_CAP = 5,
  QUEEN_PROM = 6,
  ROOK_PROM = 7,
  BISHOP_PROM = 8,
  KNIGHT_PROM = 9,
  // For captures + promotions
  CAP_QUEEN_PROM = 10,
  CAP_ROOK_PROM = 11,
  CAP_BISHOP_PROM = 12,
  CAP_KNIGHT_PROM = 13
};

inline constexpr Square get_move_source_sq(Move mv) {
  return static_cast<Square>(mv & 0x3F);
}

inline constexpr Square get_move_target_sq(Move mv) {
  return static_cast<Square>((mv >> 6) & 0x3F);
}

inline constexpr MoveFlag get_move_flag(Move mv) {
  return static_cast<MoveFlag>((mv >> 12) & 0x0F);
}

inline constexpr Move encode_move(Square source_sq, Square target_sq,
                                  MoveFlag flag) {
  return static_cast<Move>((flag << 12) | (target_sq << 6) | source_sq);
}

void print_move(Move mv);