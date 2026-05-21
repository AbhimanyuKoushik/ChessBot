#pragma once

#include <movedef.hpp>
#include <movegen.hpp>
#include <types.hpp>

template <Piece slider_piece>

inline void generate_slider_moves(const Board& board, MoveList& movelist,
                                  Bitboard target_mask) {
  Color our_side = board.get_side_to_move();
  Color opp_side = (our_side == WHITE) ? BLACK : WHITE;
  Bitboard slider_board = board.get_piece_bitboard(slider_piece);

  Bitboard our_pieces = board.get_side_occupancy(our_side);
  Bitboard enemy_pieces = board.get_side_occupancy(opp_side);
  Bitboard full_occupancy = our_pieces | enemy_pieces;

  Bitboard slider_attacks = 0ULL, valid_attacks = 0ULL;
  Square source_sq, target_sq;

  while (slider_board) {
    source_sq = static_cast<Square>(get_lsb_index(slider_board));

    if constexpr (slider_piece == W_ROOK || slider_piece == B_ROOK) {
      slider_attacks = RookAttacks(full_occupancy, source_sq);
    } else if constexpr (slider_piece == W_BISHOP || slider_piece == B_BISHOP) {
      slider_attacks = BishopAttacks(full_occupancy, source_sq);
    } else if constexpr (slider_piece == W_QUEEN || slider_piece == B_QUEEN) {
      slider_attacks = QueenAttacks(full_occupancy, source_sq);
    }

    valid_attacks = slider_attacks & target_mask & ~our_pieces;

    while (valid_attacks) {
      target_sq = static_cast<Square>(get_lsb_index(valid_attacks));

      if ((1ULL << target_sq) & enemy_pieces) {
        movelist.push_back(encode_move(source_sq, target_sq, CAPTURE));
      } else {
        movelist.push_back(encode_move(source_sq, target_sq, QUIET));
      }

      pop_bit(valid_attacks, target_sq);
    }
    pop_bit(slider_board, source_sq);
  }
}