#pragma once

#include <search.hpp>
#include <types.hpp>

inline constexpr std::array<int, NB_PIECE_TYPES> generic_piece_values = {
    100, 300, 300, 500, 900, 10000};

inline void score_moves(const Board& board, MoveList& movelist, Move tt_move,
                        SearchInfo& info) {
  for (uint8_t i = 0; i < movelist.count; i++) {
    Move move = movelist.moves[i];

    if (move == tt_move) {
      movelist.scores[i] = 30000;
      continue;
    }

    else if (is_move_capture(move)) {
      Square source_sq = get_move_source_sq(move);
      Square target_sq = get_move_target_sq(move);

      Piece attacker = board.get_piece_at_square(source_sq);
      Piece victim = board.get_piece_at_square(target_sq);

      PieceType attacker_type = static_cast<PieceType>(
          attacker >= B_PAWN ? attacker - B_PAWN : attacker);
      int attacker_val = generic_piece_values[attacker_type];
      int victim_val;

      if (victim == NB_PIECES) {
        // Only for enpassant
        victim_val = generic_piece_values[PAWN];
      } else {
        PieceType victim_type =
            static_cast<PieceType>(victim >= B_PAWN ? victim - B_PAWN : victim);
        victim_val = generic_piece_values[victim_type];
      }

      movelist.scores[i] = 10000 + (victim_val * 10) - attacker_val;
    }

    else {
      Square source_sq = get_move_source_sq(move);
      Square target_sq = get_move_target_sq(move);
      Piece piece_moving = board.get_piece_at_square(source_sq);
      movelist.scores[i] = info.historyTable[piece_moving][target_sq];
    }
  }
}