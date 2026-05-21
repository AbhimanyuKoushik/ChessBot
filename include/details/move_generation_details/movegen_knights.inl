#pragma once

#include <movedef.hpp>
#include <movegen.hpp>
#include <types.hpp>

inline void generate_knight_moves(const Board& board, MoveList& movelist,
                                  Bitboard target_mask) {
  Color our_side = board.get_side_to_move();
  Color opp_side = (our_side == WHITE ? BLACK : WHITE);
  Piece our_knight = (our_side == WHITE) ? W_KNIGHT : B_KNIGHT;
  Bitboard knights_on_board = board.get_piece_bitboard(our_knight);
  Bitboard enemy_pieces = board.get_side_occupancy(opp_side);
  Bitboard our_pieces = board.get_side_occupancy(our_side);
  Bitboard knight_attacks;
  Square source_sq, target_sq;

  while (knights_on_board) {
    source_sq = static_cast<Square>(get_lsb_index(knights_on_board));
    knight_attacks = target_mask & kKnightAttacks[source_sq] & ~our_pieces;

    while (knight_attacks) {
      target_sq = static_cast<Square>(get_lsb_index(knight_attacks));
      if ((1ULL << target_sq) & enemy_pieces) {
        movelist.push_back(encode_move(source_sq, target_sq, CAPTURE));
      } else {
        movelist.push_back(encode_move(source_sq, target_sq, QUIET));
      }
      pop_bit(knight_attacks, target_sq);
    }
    pop_bit(knights_on_board, source_sq);
  }
}