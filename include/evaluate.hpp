#pragma once

#include <board.hpp>

inline int evaluate(const Board& board) {
  int mg_score = board.get_current_middlegame_score();
  int eg_score = board.get_current_endgame_score();
  int phase = board.get_current_phase();

  // capping phase else it will invert tapered scor
  if (phase > 24) phase = 24;
  // tapered score
  int score = (mg_score * phase + eg_score * (24 - phase)) / 24;
  return (board.get_side_to_move() == WHITE ? score : -score);
}