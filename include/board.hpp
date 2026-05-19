#pragma once

#include <array>
#include <bitboards.hpp>
#include <leapers.hpp>
#include <movedef.hpp>
#include <sliders.hpp>
#include <types.hpp>

/*
StateInfo contains the following information -
Piece captured in the previous move
Enpassent square, can be either none or some square
Castling previliges for each side
Number of half moves, or in other words plies

CastingRights -> if first bit is 1, means white can kingside castle
if next bit is 1, means white can queenside castle
Similar for next 2 bits for black
*/

struct StateInfo {
  Piece capturedPiece;
  Square enpassentSquare;
  uint8_t castlingRights;
  uint8_t halfmoveClock;
};

class Board {
 private:
  std::array<StateInfo, 1024> stateHistory;
  std::array<Bitboard, 12> pieceBitboards;
  std::array<Bitboard, 3> sideOccupancies;
  std::array<Piece, 64> pieceOnSquare;

  Color currentSideToMove;
  Square currentEnpassentSquare;
  uint8_t currentCastlingRights;
  Piece pieceCapturedPreviously;
  uint8_t currentHalfmoveClock;
  uint16_t currentPly;

 public:
  inline void make_move(Move mv);

  // Have to input move in undo move, otherwise have to store in StateInfo
  // struct, which basically doubles the struct size
  inline void undo_move(Move mv);

  // The question is, is the position legal Position after the move is made is
  // not legal is same as saying move is not legal
  inline bool is_position_legal();

  // This function checks if the given side attacks a square or not
  inline bool is_square_attacked(Square sq, Color side) const;

  inline Bitboard get_piece_bitboard(Piece p) const;

  inline Piece get_piece_at_square(Square sq) const;

  inline Color get_side_to_move() const;

  inline Bitboard get_side_occupancy(Color side) const;

  inline Square get_enpassent_sq() const;

  inline uint8_t get_castling_rights() const;

  // constructor
  inline Board();

  // utilities
  void InitializeBoard();
  void LoadFEN(std::string fenString);
  void printBoard();
};

#include <details/board_inline.inl>