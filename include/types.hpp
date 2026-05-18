#pragma once

#include <array>
#include <cstdint>

/*
Will be using bitboards every to represent location of pieces, move generating
and stuff So to be clear when we are using bitboard we use an alias
*/
using Bitboard = uint64_t;

/*
We need 15 bitboards later on to store stuff, for each piece of each color,
white occupancies, black occupancies and both occupancies
We need enums for both piece type and piece because
piece type will be used for move encoding, PSTs and evaluation while
piece will be use for the bitboards
*/

// NB means number of
enum Color : uint8_t { WHITE, BLACK, NB_COLOR = 2 };

enum PieceType : uint8_t {
  PAWN,
  KNIGHT,
  BISHOP,
  ROOK,
  QUEEN,
  KING,
  NB_PIECE_TYPES = 6
};

// clang-format off
enum Piece : uint8_t {
  W_PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
  B_PAWN, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
  NB_PIECES = 12
};

enum Square : uint8_t {
  A8, B8, C8, D8, E8, F8, G8, H8,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A1, B1, C1, D1, E1, F1, G1, H1, NB_SQ = 64
};
// clang-format on
