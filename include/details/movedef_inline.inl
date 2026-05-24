#pragma once

#include <cstdint>
#include <movedef.hpp>
#include <types.hpp>

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

inline constexpr bool is_move_capture(Move mv) {
  MoveFlag flag = get_move_flag(mv);
  if ((flag == CAPTURE) || (flag == CAP_QUEEN_PROM) ||
      (flag == CAP_ROOK_PROM) || (flag == CAP_BISHOP_PROM) ||
      (flag == CAP_KNIGHT_PROM)) {
    return true;
  } else
    return false;
}