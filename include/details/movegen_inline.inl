#pragma once

#include <movedef.hpp>
#include <movegen.hpp>
#include <types.hpp>

inline void MoveList::push_back(Move mv) {
  moves[count] = mv;
  count++;
}

inline void generate_pseudo_legal_moves(const Board& board, MoveList& movelist,
                                        Bitboard target_mask) {
  generate_pawn_moves(board, movelist, target_mask);
  generate_knight_moves(board, movelist, target_mask);
  generate_slider_moves(board, movelist, target_mask);
  generate_king_moves(board, movelist, target_mask);
}

#include <move_generation_details/movegen_kings.inl>
#include <move_generation_details/movegen_knights.inl>
#include <move_generation_details/movegen_pawns.inl>
#include <move_generation_details/movegen_sliders.inl>