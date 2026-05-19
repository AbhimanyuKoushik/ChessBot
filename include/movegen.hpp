#pragma once

#include <array>
#include <board.hpp>
#include <movedef.hpp>

struct MoveList {
  std::array<Move, 256> moves;
  uint8_t count = 0;

  inline void push_back(Move mv);

  inline const Move* begin() const { return &moves[0]; }
  inline const Move* end() const { return &moves[count]; }

  inline Move* begin() { return &moves[0]; }
  inline Move* end() { return &moves[count]; }
};

// Generates pseudo legal moves whose target square bit is 1 in the target
// Same function can be used to generate all kind of captures, quiet moves
// When target_mask is ~sideOccupancy[ourside], then all kinds of moves are
// generated. When it is sideOccupancy[oppside] then only captures are generated
// Similarly only quiet moves can be generated
inline void generate_pseudo_legal_moves(const Board& board, MoveList& movelist,
                                        Bitboard target_mask);

// We will use individual functions for pieces and the above function will
// enclose them generating all the moves
inline void generate_pawn_moves(const Board& board, MoveList& movelist,
                                Bitboard target_mask);
inline void generate_knight_moves(const Board& board, MoveList& movelist,
                                  Bitboard target_mask);
// all sliders behave more or less in similar manner
inline void generate_slider_moves(const Board& board, MoveList& movelist,
                                  Bitboard target_mask);
inline void generate_king_moves(const Board& board, MoveList& movelist,
                                Bitboard target_mask);

// For Pawns, pushes and captures behave differently, so need to take care of
// them in different manner
inline void generate_white_pawn_pushes(const Board& board, MoveList& movelist,
                                       Bitboard target_mask,
                                       Bitboard empty_squares);
inline void generate_white_pawn_captures(const Board& board, MoveList& movelist,
                                         Bitboard target_mask,
                                         Bitboard enemy_on_squares);
inline void generate_white_ep_captures(const Board& board, MoveList& movelist);

inline void generate_black_pawn_pushes(const Board& board, MoveList& movelist,
                                       Bitboard target_mask,
                                       Bitboard empty_squares);
inline void generate_black_pawn_captures(const Board& board, MoveList& movelist,
                                         Bitboard target_mask,
                                         Bitboard enemy_on_squares);
inline void generate_black_ep_captures(const Board& board, MoveList& movelist);

#include <details/movegen_inline.inl>