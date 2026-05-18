#pragma once

#include <array>
#include <bitboards.hpp>
#include <movedef.hpp>
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

inline constexpr std::array<uint8_t, 64> castlingRights = {
    0x07, 0x0F, 0x0F, 0x0F, 0x03, 0x0F, 0x0F, 0x0B, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0D, 0x0F, 0x0F, 0x0F, 0x0C, 0x0F, 0x0F, 0x0E};

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
  inline void make_move(Move mv) {
    Square source_sq = get_move_source_sq(mv);
    Square target_sq = get_move_target_sq(mv);
    MoveFlag flag = get_move_flag(mv);
    Piece piece_moving = pieceOnSquare[source_sq];
    Piece piece_getting_captured = pieceOnSquare[target_sq];
    Color opposite_side = (currentSideToMove == WHITE) ? BLACK : WHITE;

    // Updating the history before making any changes
    StateInfo previousState;
    previousState.capturedPiece = piece_getting_captured;
    previousState.enpassentSquare = currentEnpassentSquare;
    previousState.castlingRights = currentCastlingRights;
    previousState.halfmoveClock = currentHalfmoveClock;
    stateHistory[currentPly] = previousState;

    // General updates, have to be done for any type of move
    // Simultaneously updates the source and target square bits, much faster
    // than set_bit, pop_bit functionalities
    Bitboard update_mask = ((1ULL << source_sq) | (1ULL << target_sq));
    pieceBitboards[piece_moving] ^= update_mask;
    sideOccupancies[currentSideToMove] ^= update_mask;

    currentEnpassentSquare = NB_SQ;
    currentCastlingRights &=
        castlingRights[source_sq] & castlingRights[target_sq];
    pieceOnSquare[source_sq] = NB_PIECES;
    pieceOnSquare[target_sq] = piece_moving;
    currentHalfmoveClock++;

    // Move type specific updates
    switch (flag) {
      case QUIET: {
        if (piece_moving == W_PAWN || piece_moving == B_PAWN)
          currentHalfmoveClock = 0;
        break;
      }

      case DOUBLE_PUSH: {
        if (piece_moving == W_PAWN) {
          currentEnpassentSquare = static_cast<Square>(target_sq + 8);
        } else {
          currentEnpassentSquare = static_cast<Square>(target_sq - 8);
        }
        currentHalfmoveClock = 0;
        break;
      }

      case CAPTURE: {
        currentHalfmoveClock = 0;
        Bitboard capture_mask = (1ULL << target_sq);
        pieceBitboards[piece_getting_captured] ^= capture_mask;
        sideOccupancies[opposite_side] ^= capture_mask;
        break;
      }

      case KING_CASTLE: {
        Bitboard castle_mask;
        if (currentSideToMove == WHITE) {
          castle_mask = (1ULL << H1) | (1ULL << F1);
          pieceBitboards[W_ROOK] ^= castle_mask;
          sideOccupancies[WHITE] ^= castle_mask;
          pieceOnSquare[H1] = NB_PIECES;
          pieceOnSquare[F1] = W_ROOK;
        } else {
          castle_mask = (1ULL << H8) | (1ULL << F8);
          pieceBitboards[B_ROOK] ^= castle_mask;
          sideOccupancies[BLACK] ^= castle_mask;
          pieceOnSquare[H8] = NB_PIECES;
          pieceOnSquare[F8] = B_ROOK;
        }
        break;
      }

      case QUEEN_CASTLE: {
        Bitboard castle_mask;
        if (currentSideToMove == WHITE) {
          castle_mask = (1ULL << A1) | (1ULL << D1);
          pieceBitboards[W_ROOK] ^= castle_mask;
          sideOccupancies[WHITE] ^= castle_mask;
          pieceOnSquare[A1] = NB_PIECES;
          pieceOnSquare[D1] = W_ROOK;
        } else {
          castle_mask = (1ULL << A8) | (1ULL << D8);
          pieceBitboards[B_ROOK] ^= castle_mask;
          sideOccupancies[BLACK] ^= castle_mask;
          pieceOnSquare[A8] = NB_PIECES;
          pieceOnSquare[D8] = B_ROOK;
        }
        break;
      }

      case ENPASS_CAP: {
        currentHalfmoveClock = 0;
        if (currentSideToMove == WHITE) {
          Square enpass_cap_sq = static_cast<Square>(target_sq + 8);
          pieceBitboards[B_PAWN] ^= (1ULL << enpass_cap_sq);
          sideOccupancies[BLACK] ^= (1ULL << enpass_cap_sq);
          pieceOnSquare[enpass_cap_sq] = NB_PIECES;
          stateHistory[currentPly].capturedPiece = B_PAWN;
        } else {
          Square enpass_cap_sq = static_cast<Square>(target_sq - 8);
          pieceBitboards[W_PAWN] ^= (1ULL << enpass_cap_sq);
          sideOccupancies[WHITE] ^= (1ULL << enpass_cap_sq);
          pieceOnSquare[enpass_cap_sq] = NB_PIECES;
          stateHistory[currentPly].capturedPiece = W_PAWN;
        }
        break;
      }

      case QUEEN_PROM: {
        currentHalfmoveClock = 0;
        Bitboard prom_sq_mask = (1ULL << target_sq);

        if (currentSideToMove == WHITE) {
          pieceOnSquare[target_sq] = W_QUEEN;
          pieceBitboards[W_PAWN] ^= prom_sq_mask;
          pieceBitboards[W_QUEEN] ^= prom_sq_mask;
        } else {
          pieceOnSquare[target_sq] = B_QUEEN;
          pieceBitboards[B_PAWN] ^= prom_sq_mask;
          pieceBitboards[B_QUEEN] ^= prom_sq_mask;
        }
        break;
      }

      case ROOK_PROM: {
        currentHalfmoveClock = 0;
        Bitboard prom_sq_mask = (1ULL << target_sq);

        if (currentSideToMove == WHITE) {
          pieceOnSquare[target_sq] = W_ROOK;
          pieceBitboards[W_PAWN] ^= prom_sq_mask;
          pieceBitboards[W_ROOK] ^= prom_sq_mask;
        } else {
          pieceOnSquare[target_sq] = B_ROOK;
          pieceBitboards[B_PAWN] ^= prom_sq_mask;
          pieceBitboards[B_ROOK] ^= prom_sq_mask;
        }
        break;
      }

      case BISHOP_PROM: {
        currentHalfmoveClock = 0;
        Bitboard prom_sq_mask = (1ULL << target_sq);

        if (currentSideToMove == WHITE) {
          pieceOnSquare[target_sq] = W_BISHOP;
          pieceBitboards[W_PAWN] ^= prom_sq_mask;
          pieceBitboards[W_BISHOP] ^= prom_sq_mask;
        } else {
          pieceOnSquare[target_sq] = B_BISHOP;
          pieceBitboards[B_PAWN] ^= prom_sq_mask;
          pieceBitboards[B_BISHOP] ^= prom_sq_mask;
        }
        break;
      }

      case KNIGHT_PROM: {
        currentHalfmoveClock = 0;
        Bitboard prom_sq_mask = (1ULL << target_sq);

        if (currentSideToMove == WHITE) {
          pieceOnSquare[target_sq] = W_KNIGHT;
          pieceBitboards[W_PAWN] ^= prom_sq_mask;
          pieceBitboards[W_KNIGHT] ^= prom_sq_mask;
        } else {
          pieceOnSquare[target_sq] = B_KNIGHT;
          pieceBitboards[B_PAWN] ^= prom_sq_mask;
          pieceBitboards[B_KNIGHT] ^= prom_sq_mask;
        }
        break;
      }

      case CAP_QUEEN_PROM: {
        currentHalfmoveClock = 0;
        Bitboard prom_sq_mask = (1ULL << target_sq);

        if (currentSideToMove == WHITE) {
          pieceOnSquare[target_sq] = W_QUEEN;
          pieceBitboards[W_PAWN] ^= prom_sq_mask;
          pieceBitboards[W_QUEEN] ^= prom_sq_mask;
        } else {
          pieceOnSquare[target_sq] = B_QUEEN;
          pieceBitboards[B_PAWN] ^= prom_sq_mask;
          pieceBitboards[B_QUEEN] ^= prom_sq_mask;
        }

        pieceBitboards[piece_getting_captured] ^= prom_sq_mask;
        sideOccupancies[opposite_side] ^= prom_sq_mask;
        break;
      }

      case CAP_ROOK_PROM: {
        currentHalfmoveClock = 0;
        Bitboard prom_sq_mask = (1ULL << target_sq);

        if (currentSideToMove == WHITE) {
          pieceOnSquare[target_sq] = W_ROOK;
          pieceBitboards[W_PAWN] ^= prom_sq_mask;
          pieceBitboards[W_ROOK] ^= prom_sq_mask;
        } else {
          pieceOnSquare[target_sq] = B_ROOK;
          pieceBitboards[B_PAWN] ^= prom_sq_mask;
          pieceBitboards[B_ROOK] ^= prom_sq_mask;
        }

        pieceBitboards[piece_getting_captured] ^= prom_sq_mask;
        sideOccupancies[opposite_side] ^= prom_sq_mask;
        break;
      }

      case CAP_BISHOP_PROM: {
        currentHalfmoveClock = 0;
        Bitboard prom_sq_mask = (1ULL << target_sq);

        if (currentSideToMove == WHITE) {
          pieceOnSquare[target_sq] = W_BISHOP;
          pieceBitboards[W_PAWN] ^= prom_sq_mask;
          pieceBitboards[W_BISHOP] ^= prom_sq_mask;
        } else {
          pieceOnSquare[target_sq] = B_BISHOP;
          pieceBitboards[B_PAWN] ^= prom_sq_mask;
          pieceBitboards[B_BISHOP] ^= prom_sq_mask;
        }

        pieceBitboards[piece_getting_captured] ^= prom_sq_mask;
        sideOccupancies[opposite_side] ^= prom_sq_mask;
        break;
      }

      case CAP_KNIGHT_PROM: {
        currentHalfmoveClock = 0;
        Bitboard prom_sq_mask = (1ULL << target_sq);

        if (currentSideToMove == WHITE) {
          pieceOnSquare[target_sq] = W_KNIGHT;
          pieceBitboards[W_PAWN] ^= prom_sq_mask;
          pieceBitboards[W_KNIGHT] ^= prom_sq_mask;
        } else {
          pieceOnSquare[target_sq] = B_KNIGHT;
          pieceBitboards[B_PAWN] ^= prom_sq_mask;
          pieceBitboards[B_KNIGHT] ^= prom_sq_mask;
        }

        pieceBitboards[piece_getting_captured] ^= prom_sq_mask;
        sideOccupancies[opposite_side] ^= prom_sq_mask;
        break;
      }

      default:
        break;
    }

    currentPly++;
    sideOccupancies[NB_COLOR] = sideOccupancies[WHITE] | sideOccupancies[BLACK];
    currentSideToMove = (currentSideToMove == WHITE) ? BLACK : WHITE;
  }

  // Have to input move in undo move, otherwise have to store in StateInfo
  // struct, which basically doubles the struct size
  inline void undo_move(Move mv) {
    currentPly--;
    // currentSide to move is set to the previous side to move
    currentSideToMove = (currentSideToMove == WHITE) ? BLACK : WHITE;
    Color opposite_side = (currentSideToMove == WHITE) ? BLACK : WHITE;

    StateInfo previousState = stateHistory[currentPly];
    Piece captured_piece = previousState.capturedPiece;
    currentEnpassentSquare = previousState.enpassentSquare;
    currentCastlingRights = previousState.castlingRights;
    currentHalfmoveClock = previousState.halfmoveClock;

    Square previous_source_sq = get_move_source_sq(mv);
    Square previous_target_sq = get_move_target_sq(mv);
    MoveFlag previous_flag = get_move_flag(mv);
    Piece moved_piece = pieceOnSquare[previous_target_sq];

    pieceOnSquare[previous_target_sq] = captured_piece;
    pieceOnSquare[previous_source_sq] = moved_piece;

    // Reversing the flag specific updates
    switch (previous_flag) {
      case CAPTURE: {
        Bitboard prev_capture_mask = (1ULL << previous_target_sq);
        pieceBitboards[captured_piece] ^= prev_capture_mask;
        sideOccupancies[opposite_side] ^= prev_capture_mask;
        break;
      }

      case KING_CASTLE: {
        Bitboard prev_castle_mask;
        if (currentSideToMove == WHITE) {
          prev_castle_mask = (1ULL << H1) | (1ULL << F1);
          pieceBitboards[W_ROOK] ^= prev_castle_mask;
          sideOccupancies[WHITE] ^= prev_castle_mask;
          pieceOnSquare[H1] = W_ROOK;
          pieceOnSquare[F1] = NB_PIECES;
        } else {
          prev_castle_mask = (1ULL << H8) | (1ULL << F8);
          pieceBitboards[B_ROOK] ^= prev_castle_mask;
          sideOccupancies[BLACK] ^= prev_castle_mask;
          pieceOnSquare[H8] = B_ROOK;
          pieceOnSquare[F8] = NB_PIECES;
        }
        break;
      }

      case QUEEN_CASTLE: {
        Bitboard prev_castle_mask;
        if (currentSideToMove == WHITE) {
          prev_castle_mask = (1ULL << A1) | (1ULL << D1);
          pieceBitboards[W_ROOK] ^= prev_castle_mask;
          sideOccupancies[WHITE] ^= prev_castle_mask;
          pieceOnSquare[A1] = W_ROOK;
          pieceOnSquare[D1] = NB_PIECES;
        } else {
          prev_castle_mask = (1ULL << A8) | (1ULL << D8);
          pieceBitboards[B_ROOK] ^= prev_castle_mask;
          sideOccupancies[BLACK] ^= prev_castle_mask;
          pieceOnSquare[A8] = B_ROOK;
          pieceOnSquare[D8] = NB_PIECES;
        }
        break;
      }

      case ENPASS_CAP: {
        if (currentSideToMove == WHITE) {
          Square prev_enpass_cap_sq =
              static_cast<Square>(previous_target_sq + 8);
          pieceBitboards[B_PAWN] ^= (1ULL << prev_enpass_cap_sq);
          sideOccupancies[BLACK] ^= (1ULL << prev_enpass_cap_sq);
          pieceOnSquare[previous_target_sq] = NB_PIECES;
          pieceOnSquare[prev_enpass_cap_sq] = B_PAWN;
        } else {
          Square prev_enpass_cap_sq =
              static_cast<Square>(previous_target_sq - 8);
          pieceBitboards[W_PAWN] ^= (1ULL << prev_enpass_cap_sq);
          sideOccupancies[WHITE] ^= (1ULL << prev_enpass_cap_sq);
          pieceOnSquare[previous_target_sq] = NB_PIECES;
          pieceOnSquare[prev_enpass_cap_sq] = W_PAWN;
        }
        break;
      }

      case QUEEN_PROM: {
        Bitboard prom_sq_mask = (1ULL << previous_target_sq);

        if (currentSideToMove == WHITE) {
          pieceOnSquare[previous_source_sq] = W_PAWN;
          pieceBitboards[W_PAWN] ^= prom_sq_mask;
          pieceBitboards[W_QUEEN] ^= prom_sq_mask;
          moved_piece = W_PAWN;
        } else {
          pieceOnSquare[previous_source_sq] = B_PAWN;
          pieceBitboards[B_PAWN] ^= prom_sq_mask;
          pieceBitboards[B_QUEEN] ^= prom_sq_mask;
          moved_piece = B_PAWN;
        }
        break;
      }

      case ROOK_PROM: {
        Bitboard prom_sq_mask = (1ULL << previous_target_sq);

        if (currentSideToMove == WHITE) {
          pieceOnSquare[previous_source_sq] = W_PAWN;
          pieceBitboards[W_PAWN] ^= prom_sq_mask;
          pieceBitboards[W_ROOK] ^= prom_sq_mask;
          moved_piece = W_PAWN;
        } else {
          pieceOnSquare[previous_source_sq] = B_PAWN;
          pieceBitboards[B_PAWN] ^= prom_sq_mask;
          pieceBitboards[B_ROOK] ^= prom_sq_mask;
          moved_piece = B_PAWN;
        }
        break;
      }

      case BISHOP_PROM: {
        Bitboard prom_sq_mask = (1ULL << previous_target_sq);

        if (currentSideToMove == WHITE) {
          pieceOnSquare[previous_source_sq] = W_PAWN;
          pieceBitboards[W_PAWN] ^= prom_sq_mask;
          pieceBitboards[W_BISHOP] ^= prom_sq_mask;
          moved_piece = W_PAWN;
        } else {
          pieceOnSquare[previous_source_sq] = B_PAWN;
          pieceBitboards[B_PAWN] ^= prom_sq_mask;
          pieceBitboards[B_BISHOP] ^= prom_sq_mask;
          moved_piece = B_PAWN;
        }
        break;
      }

      case KNIGHT_PROM: {
        Bitboard prom_sq_mask = (1ULL << previous_target_sq);

        if (currentSideToMove == WHITE) {
          pieceOnSquare[previous_source_sq] = W_PAWN;
          pieceBitboards[W_PAWN] ^= prom_sq_mask;
          pieceBitboards[W_KNIGHT] ^= prom_sq_mask;
          moved_piece = W_PAWN;
        } else {
          pieceOnSquare[previous_source_sq] = B_PAWN;
          pieceBitboards[B_PAWN] ^= prom_sq_mask;
          pieceBitboards[B_KNIGHT] ^= prom_sq_mask;
          moved_piece = B_PAWN;
        }
        break;
      }

      case CAP_QUEEN_PROM: {
        Bitboard prom_sq_mask = (1ULL << previous_target_sq);

        if (currentSideToMove == WHITE) {
          pieceOnSquare[previous_source_sq] = W_PAWN;
          pieceBitboards[W_PAWN] ^= prom_sq_mask;
          pieceBitboards[W_QUEEN] ^= prom_sq_mask;
          moved_piece = W_PAWN;
        } else {
          pieceOnSquare[previous_source_sq] = B_PAWN;
          pieceBitboards[B_PAWN] ^= prom_sq_mask;
          pieceBitboards[B_QUEEN] ^= prom_sq_mask;
          moved_piece = B_PAWN;
        }

        pieceBitboards[captured_piece] ^= prom_sq_mask;
        sideOccupancies[opposite_side] ^= prom_sq_mask;
        break;
      }

      case CAP_ROOK_PROM: {
        Bitboard prom_sq_mask = (1ULL << previous_target_sq);

        if (currentSideToMove == WHITE) {
          pieceOnSquare[previous_source_sq] = W_PAWN;
          pieceBitboards[W_PAWN] ^= prom_sq_mask;
          pieceBitboards[W_ROOK] ^= prom_sq_mask;
          moved_piece = W_PAWN;
        } else {
          pieceOnSquare[previous_source_sq] = B_PAWN;
          pieceBitboards[B_PAWN] ^= prom_sq_mask;
          pieceBitboards[B_ROOK] ^= prom_sq_mask;
          moved_piece = B_PAWN;
        }

        pieceBitboards[captured_piece] ^= prom_sq_mask;
        sideOccupancies[opposite_side] ^= prom_sq_mask;
        break;
      }

      case CAP_BISHOP_PROM: {
        Bitboard prom_sq_mask = (1ULL << previous_target_sq);

        if (currentSideToMove == WHITE) {
          pieceOnSquare[previous_source_sq] = W_PAWN;
          pieceBitboards[W_PAWN] ^= prom_sq_mask;
          pieceBitboards[W_BISHOP] ^= prom_sq_mask;
          moved_piece = W_PAWN;
        } else {
          pieceOnSquare[previous_source_sq] = B_PAWN;
          pieceBitboards[B_PAWN] ^= prom_sq_mask;
          pieceBitboards[B_BISHOP] ^= prom_sq_mask;
          moved_piece = B_PAWN;
        }

        pieceBitboards[captured_piece] ^= prom_sq_mask;
        sideOccupancies[opposite_side] ^= prom_sq_mask;
        break;
      }

      case CAP_KNIGHT_PROM: {
        Bitboard prom_sq_mask = (1ULL << previous_target_sq);

        if (currentSideToMove == WHITE) {
          pieceOnSquare[previous_source_sq] = W_PAWN;
          pieceBitboards[W_PAWN] ^= prom_sq_mask;
          pieceBitboards[W_KNIGHT] ^= prom_sq_mask;
          moved_piece = W_PAWN;
        } else {
          pieceOnSquare[previous_source_sq] = B_PAWN;
          pieceBitboards[B_PAWN] ^= prom_sq_mask;
          pieceBitboards[B_KNIGHT] ^= prom_sq_mask;
          moved_piece = B_PAWN;
        }

        pieceBitboards[captured_piece] ^= prom_sq_mask;
        sideOccupancies[opposite_side] ^= prom_sq_mask;
        break;
      }

      default:
        break;
    }

    // Reversing the common updates
    Bitboard update_mask =
        ((1ULL << previous_source_sq) | (1ULL << previous_target_sq));
    pieceBitboards[moved_piece] ^= update_mask;
    sideOccupancies[currentSideToMove] ^= update_mask;
    sideOccupancies[NB_COLOR] = sideOccupancies[WHITE] | sideOccupancies[BLACK];
  }

  inline bool is_move_legal(Move mv) {}

  inline Bitboard get_piece_bitboard(Piece p) { return pieceBitboards[p]; }

  inline Piece get_piece_at_square(Square sq) { return pieceOnSquare[sq]; }

  inline Color get_side_to_move() { return currentSideToMove; }

  Board() {
    StateInfo emptyInfo = {NB_PIECES, NB_SQ, 0, 0};
    stateHistory.fill(emptyInfo);
    pieceBitboards.fill(0ULL);
    sideOccupancies.fill(0ULL);
    pieceOnSquare.fill(NB_PIECES);

    currentSideToMove = NB_COLOR;
    pieceCapturedPreviously = NB_PIECES;
    currentEnpassentSquare = NB_SQ;
    currentCastlingRights = 0;
    currentHalfmoveClock = 0;
  }

  void InitializeBoard();
  void LoadFEN(std::string fenString);
  void printBoard();
};