#pragma once

#include <movedef.hpp>
#include <movegen.hpp>
#include <types.hpp>

inline void generate_king_moves(const Board& board, MoveList& movelist,
                                Bitboard target_mask) {
  Color our_side = board.get_side_to_move();
  Color opp_side = (our_side == WHITE ? BLACK : WHITE);
  Piece our_king = (our_side == WHITE) ? W_KING : B_KING;

  Bitboard king_bitboard = board.get_piece_bitboard(our_king);
  Bitboard our_pieces = board.get_side_occupancy(our_side);
  Bitboard enemy_pieces = board.get_side_occupancy(opp_side);
  Bitboard full_occupancy = our_pieces | enemy_pieces;

  Bitboard king_attacks;
  Square source_sq, target_sq;

  source_sq = static_cast<Square>(get_lsb_index(king_bitboard));
  king_attacks = target_mask & kKingAttacks[source_sq] & ~our_pieces;

  while (king_attacks) {
    target_sq = static_cast<Square>(get_lsb_index(king_attacks));
    if ((1ULL << target_sq) & enemy_pieces) {
      movelist.push_back(encode_move(source_sq, target_sq, CAPTURE));
    } else {
      movelist.push_back(encode_move(source_sq, target_sq, QUIET));
    }
    pop_bit(king_attacks, target_sq);
  }

  uint8_t castling_rights = board.get_castling_rights();
  if (castling_rights == 0) return;

  if (our_side == WHITE) {
    // White Kingside
    if (castling_rights & 0x01) {
      if (!(full_occupancy & ((1ULL << F1) | (1ULL << G1)))) {
        if (target_mask & (1ULL << G1)) {
          if (!board.is_square_attacked(E1, opp_side) &&
              !board.is_square_attacked(F1, opp_side)) {
            movelist.push_back(encode_move(E1, G1, KING_CASTLE));
          }
        }
      }
    }

    // White Queenside
    if (castling_rights & 0x02) {
      if (!(full_occupancy & ((1ULL << B1) | (1ULL << C1) | (1ULL << D1)))) {
        if (target_mask & (1ULL << C1)) {
          // Check E1 (start) and D1 (pass-through)
          if (!board.is_square_attacked(E1, opp_side) &&
              !board.is_square_attacked(D1, opp_side)) {
            movelist.push_back(encode_move(E1, C1, QUEEN_CASTLE));
          }
        }
      }
    }
  } else {
    // Black Kingside
    if (castling_rights & 0x04) {
      if (!(full_occupancy & ((1ULL << F8) | (1ULL << G8)))) {
        if (target_mask & (1ULL << G8)) {
          if (!board.is_square_attacked(E8, opp_side) &&
              !board.is_square_attacked(F8, opp_side)) {
            movelist.push_back(encode_move(E8, G8, KING_CASTLE));
          }
        }
      }
    }

    // Black Queenside
    if (castling_rights & 0x08) {  // Fixed to 0x08
      if (!(full_occupancy & ((1ULL << B8) | (1ULL << C8) | (1ULL << D8)))) {
        if (target_mask & (1ULL << C8)) {
          // Check E8 (start) and D8 (pass-through)
          if (!board.is_square_attacked(E8, opp_side) &&
              !board.is_square_attacked(D8, opp_side)) {
            movelist.push_back(encode_move(E8, C8, QUEEN_CASTLE));
          }
        }
      }
    }
  }
}