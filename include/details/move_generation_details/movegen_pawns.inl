#pragma once

#include <movedef.hpp>
#include <movegen.hpp>
#include <types.hpp>

inline void generate_pawn_moves(const Board& board, MoveList& movelist,
                                Bitboard target_mask) {
  Color our_side = board.get_side_to_move();
  Bitboard empty_squares = ~(board.get_side_occupancy(NB_COLOR));
  Color opp_side = ((our_side == WHITE) ? BLACK : WHITE);
  Bitboard enemy_on_squares = board.get_side_occupancy(opp_side);

  if (our_side == WHITE) {
    generate_white_pawn_pushes(board, movelist, target_mask, empty_squares);
    generate_white_pawn_captures(board, movelist, target_mask,
                                 enemy_on_squares);
    generate_white_ep_captures(board, movelist);
  } else {
    generate_black_pawn_pushes(board, movelist, target_mask, empty_squares);
    generate_black_pawn_captures(board, movelist, target_mask,
                                 enemy_on_squares);
    generate_black_ep_captures(board, movelist);
  }
}

inline void generate_white_pawn_pushes(const Board& board, MoveList& movelist,
                                       Bitboard target_mask,
                                       Bitboard empty_squares) {
  // For the pawn to be pushed, it should be empty (decided by the empty_square)
  // By shifting up all the rows by 1 file, we get all the pawn pushes in a
  // single go. lets call this all_pawns_pushed
  // Now we get all the possible shifts by anding all_pawns_pushed and possible
  // squares for pushing
  Bitboard pawn_on_squares = board.get_piece_bitboard(W_PAWN);
  Bitboard all_single_pushes = (pawn_on_squares >> 8) & empty_squares;
  Bitboard possible_single_pushes = all_single_pushes & target_mask;

  // For double we AND the possible single with 3rd rank, so that if pawn is
  // able to move to 3rd rank, then if the 4th rank is possible for push it must
  // be able to move there as well
  Bitboard all_double_pushes =
      ((all_single_pushes & 0x00'00'FF'00'00'00'00'00) >> 8) & empty_squares;
  Bitboard possible_double_pushes = all_double_pushes & target_mask;

  // Now we will start encoding these
  Square source_sq, target_sq;
  while (possible_single_pushes) {
    target_sq = static_cast<Square>(get_lsb_index(possible_single_pushes));
    source_sq = static_cast<Square>(target_sq + 8);  // Source is below target

    if (target_sq <= H8) {
      // Should also have target >= A8 but A8 is any encoded as 0
      movelist.push_back(encode_move(source_sq, target_sq, QUEEN_PROM));
      movelist.push_back(encode_move(source_sq, target_sq, ROOK_PROM));
      movelist.push_back(encode_move(source_sq, target_sq, BISHOP_PROM));
      movelist.push_back(encode_move(source_sq, target_sq, KNIGHT_PROM));
    } else
      movelist.push_back(encode_move(source_sq, target_sq, QUIET));

    pop_bit(possible_single_pushes, target_sq);
  }

  while (possible_double_pushes) {
    target_sq = static_cast<Square>(get_lsb_index(possible_double_pushes));
    source_sq = static_cast<Square>(target_sq + 16);  // Source is below target
    movelist.push_back(encode_move(source_sq, target_sq, DOUBLE_PUSH));

    pop_bit(possible_double_pushes, target_sq);
  }
}

inline void generate_white_pawn_captures(const Board& board, MoveList& movelist,
                                         Bitboard target_mask,
                                         Bitboard enemy_on_squares) {
  Bitboard pawn_on_squares = board.get_piece_bitboard(W_PAWN);
  Bitboard valid_targets = target_mask & enemy_on_squares;
  Bitboard left_captures = (pawn_on_squares >> 9) & valid_targets;
  Bitboard right_captures = (pawn_on_squares >> 7) & valid_targets;

  // Left captures can never be on H file, right captures can never be on A file
  left_captures &= kNotHFile;
  right_captures &= kNotAFile;

  Square source_sq, target_sq;

  while (left_captures) {
    target_sq = static_cast<Square>(get_lsb_index(left_captures));
    source_sq = static_cast<Square>((target_sq + 9));

    if (target_sq <= H8) {
      movelist.push_back(encode_move(source_sq, target_sq, CAP_QUEEN_PROM));
      movelist.push_back(encode_move(source_sq, target_sq, CAP_ROOK_PROM));
      movelist.push_back(encode_move(source_sq, target_sq, CAP_BISHOP_PROM));
      movelist.push_back(encode_move(source_sq, target_sq, CAP_KNIGHT_PROM));
    } else {
      movelist.push_back(encode_move(source_sq, target_sq, CAPTURE));
    }

    pop_bit(left_captures, target_sq);
  }

  while (right_captures) {
    target_sq = static_cast<Square>(get_lsb_index(right_captures));
    source_sq = static_cast<Square>((target_sq + 7));

    if (target_sq <= H8) {
      movelist.push_back(encode_move(source_sq, target_sq, CAP_QUEEN_PROM));
      movelist.push_back(encode_move(source_sq, target_sq, CAP_ROOK_PROM));
      movelist.push_back(encode_move(source_sq, target_sq, CAP_BISHOP_PROM));
      movelist.push_back(encode_move(source_sq, target_sq, CAP_KNIGHT_PROM));
    } else {
      movelist.push_back(encode_move(source_sq, target_sq, CAPTURE));
    }

    pop_bit(right_captures, target_sq);
  }
}

inline void generate_white_ep_captures(const Board& board, MoveList& movelist) {
  Square enpass_sq = board.get_enpassent_sq();

  // No enpassent square then done
  if (enpass_sq == NB_SQ) return;

  Bitboard enpass_mask = (1ULL << enpass_sq);

  // Pawns on A file cannot enpass left, pawns on H file same on right
  Bitboard our_pawns = board.get_piece_bitboard(W_PAWN);
  Bitboard enpass_left = ((our_pawns & kNotAFile) >> 9) & enpass_mask;
  Bitboard enpass_right = ((our_pawns & kNotHFile) >> 7) & enpass_mask;

  Square source_sq, target_sq;
  while (enpass_left) {
    target_sq = static_cast<Square>(get_lsb_index(enpass_left));
    source_sq = static_cast<Square>((target_sq + 9));
    movelist.push_back(encode_move(source_sq, target_sq, ENPASS_CAP));
    pop_bit(enpass_left, target_sq);
  }

  while (enpass_right) {
    target_sq = static_cast<Square>(get_lsb_index(enpass_right));
    source_sq = static_cast<Square>((target_sq + 7));
    movelist.push_back(encode_move(source_sq, target_sq, ENPASS_CAP));
    pop_bit(enpass_right, target_sq);
  }
}

inline void generate_black_pawn_pushes(const Board& board, MoveList& movelist,
                                       Bitboard target_mask,
                                       Bitboard empty_squares) {
  // Now its shifting down all the rows by 1 file.
  Bitboard pawn_on_squares = board.get_piece_bitboard(B_PAWN);
  Bitboard all_single_pushes = (pawn_on_squares << 8) & empty_squares;
  Bitboard possible_single_pushes = all_single_pushes & target_mask;

  // For double we AND the possible single with 3rd rank, so that if pawn is
  // able to move to 3rd rank, then if the 4th rank is possible for push it must
  // be able to move there as well
  Bitboard all_double_pushes =
      ((all_single_pushes & 0x00'00'00'00'00'FF'00'00) << 8) & empty_squares;
  Bitboard possible_double_pushes = all_double_pushes & target_mask;

  // Now we will start encoding these
  Square source_sq, target_sq;
  while (possible_single_pushes) {
    target_sq = static_cast<Square>(get_lsb_index(possible_single_pushes));
    source_sq = static_cast<Square>(target_sq - 8);  // Source is below target

    if (target_sq >= A1) {
      // Should also have target <= H1 but max is H1 anyway
      movelist.push_back(encode_move(source_sq, target_sq, QUEEN_PROM));
      movelist.push_back(encode_move(source_sq, target_sq, ROOK_PROM));
      movelist.push_back(encode_move(source_sq, target_sq, BISHOP_PROM));
      movelist.push_back(encode_move(source_sq, target_sq, KNIGHT_PROM));
    } else
      movelist.push_back(encode_move(source_sq, target_sq, QUIET));

    pop_bit(possible_single_pushes, target_sq);
  }

  while (possible_double_pushes) {
    target_sq = static_cast<Square>(get_lsb_index(possible_double_pushes));
    source_sq = static_cast<Square>(target_sq - 16);
    movelist.push_back(encode_move(source_sq, target_sq, DOUBLE_PUSH));

    pop_bit(possible_double_pushes, target_sq);
  }
}

inline void generate_black_pawn_captures(const Board& board, MoveList& movelist,
                                         Bitboard target_mask,
                                         Bitboard enemy_on_squares) {
  Bitboard pawn_on_squares = board.get_piece_bitboard(B_PAWN);
  Bitboard valid_targets = target_mask & enemy_on_squares;
  Bitboard left_captures = (pawn_on_squares << 7) & valid_targets;
  Bitboard right_captures = (pawn_on_squares << 9) & valid_targets;

  // Left captures can never be on H file, right captures can never be on A file
  left_captures &= kNotHFile;
  right_captures &= kNotAFile;

  Square source_sq, target_sq;

  while (left_captures) {
    target_sq = static_cast<Square>(get_lsb_index(left_captures));
    source_sq = static_cast<Square>((target_sq - 7));

    if (target_sq >= A1) {
      movelist.push_back(encode_move(source_sq, target_sq, CAP_QUEEN_PROM));
      movelist.push_back(encode_move(source_sq, target_sq, CAP_ROOK_PROM));
      movelist.push_back(encode_move(source_sq, target_sq, CAP_BISHOP_PROM));
      movelist.push_back(encode_move(source_sq, target_sq, CAP_KNIGHT_PROM));
    } else {
      movelist.push_back(encode_move(source_sq, target_sq, CAPTURE));
    }

    pop_bit(left_captures, target_sq);
  }

  while (right_captures) {
    target_sq = static_cast<Square>(get_lsb_index(right_captures));
    source_sq = static_cast<Square>((target_sq - 9));

    if (target_sq >= A1) {
      movelist.push_back(encode_move(source_sq, target_sq, CAP_QUEEN_PROM));
      movelist.push_back(encode_move(source_sq, target_sq, CAP_ROOK_PROM));
      movelist.push_back(encode_move(source_sq, target_sq, CAP_BISHOP_PROM));
      movelist.push_back(encode_move(source_sq, target_sq, CAP_KNIGHT_PROM));
    } else {
      movelist.push_back(encode_move(source_sq, target_sq, CAPTURE));
    }

    pop_bit(right_captures, target_sq);
  }
}

inline void generate_black_ep_captures(const Board& board, MoveList& movelist) {
  Square enpass_sq = board.get_enpassent_sq();

  // No enpassent square then done
  if (enpass_sq == NB_SQ) return;

  Bitboard enpass_mask = (1ULL << enpass_sq);

  // Pawns on A file cannot enpass left, pawns on H file same on right
  Bitboard our_pawns = board.get_piece_bitboard(B_PAWN);
  Bitboard enpass_left = ((our_pawns & kNotAFile) << 7) & enpass_mask;
  Bitboard enpass_right = ((our_pawns & kNotHFile) << 9) & enpass_mask;

  Square source_sq, target_sq;
  while (enpass_left) {
    target_sq = static_cast<Square>(get_lsb_index(enpass_left));
    source_sq = static_cast<Square>((target_sq - 7));
    movelist.push_back(encode_move(source_sq, target_sq, ENPASS_CAP));
    pop_bit(enpass_left, target_sq);
  }

  while (enpass_right) {
    target_sq = static_cast<Square>(get_lsb_index(enpass_right));
    source_sq = static_cast<Square>((target_sq - 9));
    movelist.push_back(encode_move(source_sq, target_sq, ENPASS_CAP));
    pop_bit(enpass_right, target_sq);
  }
}