#pragma once

#include <array>
#include <board.hpp>
#include <details/pesto.inl>

inline constexpr std::array<uint8_t, 64> castlingRights = {
    0x07, 0x0F, 0x0F, 0x0F, 0x03, 0x0F, 0x0F, 0x0B, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0D, 0x0F, 0x0F, 0x0F, 0x0C, 0x0F, 0x0F, 0x0E};

inline void Board::make_move(Move mv) {
  Square source_sq = get_move_source_sq(mv);
  Square target_sq = get_move_target_sq(mv);
  MoveFlag flag = get_move_flag(mv);
  Piece piece_moving = pieceOnSquare[source_sq];
  Piece piece_getting_captured = pieceOnSquare[target_sq];
  PieceType piece_type_moving = static_cast<PieceType>(
      (piece_moving >= B_PAWN) ? piece_moving - B_PAWN : piece_moving);
  PieceType piece_type_getting_captured = static_cast<PieceType>(
      (piece_getting_captured >= B_PAWN) ? piece_getting_captured - B_PAWN
                                         : piece_getting_captured);
  Color opposite_side = (currentSideToMove == WHITE) ? BLACK : WHITE;

  // Updating the history before making any changes
  StateInfo previousState;
  previousState.capturedPiece = piece_getting_captured;
  previousState.enpassentSquare = currentEnpassentSquare;
  previousState.castlingRights = currentCastlingRights;
  previousState.halfmoveClock = currentHalfmoveClock;
  previousState.hashValue = currentHashValue;
  previousState.middlegameScore = currentMiddlegameScore;
  previousState.endgameScore = currentEndgameScore;
  previousState.phase = currentPhase;
  stateHistory[currentPly] = previousState;

  // General updates, have to be done for any type of move
  // Simultaneously updates the source and target square bits, much faster
  // than set_bit, pop_bit functionalities
  Bitboard update_mask = ((1ULL << source_sq) | (1ULL << target_sq));
  pieceBitboards[piece_moving] ^= update_mask;
  sideOccupancies[currentSideToMove] ^= update_mask;

  currentHashValue ^= zkeys.castling[currentCastlingRights];
  currentHashValue ^= (currentEnpassentSquare == NB_SQ)
                          ? 0
                          : zkeys.enpassentFile[currentEnpassentSquare % 8];
  currentHashValue ^= zkeys.pieces[piece_moving][source_sq];
  currentHashValue ^= zkeys.pieces[piece_moving][target_sq];

  currentEnpassentSquare = NB_SQ;
  currentCastlingRights &=
      castlingRights[source_sq] & castlingRights[target_sq];
  pieceOnSquare[source_sq] = NB_PIECES;
  pieceOnSquare[target_sq] = piece_moving;
  currentHalfmoveClock++;

  Square flipped_source_sq = static_cast<Square>(
      (currentSideToMove == WHITE) ? source_sq : FLIP(source_sq));
  Square flipped_target_sq = static_cast<Square>(
      (currentSideToMove == WHITE) ? target_sq : FLIP(target_sq));
  // When a capture happens, you have to remove the score from victims
  // perspective If a white knight is capture (source and target square are
  // flipped because its black to move), we have to remove score by keeping with
  // respect to white, not black
  Square flipped_victim_sq = static_cast<Square>(
      (currentSideToMove == WHITE) ? FLIP(target_sq) : target_sq);

  // Score is always calculated from white's perspective
  // Hence for black we have to do what we do for white
  // subtract in place of add for black and vice versa
  // The multiplier takes care such that we write code in white's perspective
  short int multiplier = (currentSideToMove == WHITE) ? 1 : -1;
  currentMiddlegameScore -=
      multiplier * pestoMiddlegame[piece_type_moving][flipped_source_sq];
  currentMiddlegameScore +=
      multiplier * pestoMiddlegame[piece_type_moving][flipped_target_sq];
  currentEndgameScore -=
      multiplier * pestoEndgame[piece_type_moving][flipped_source_sq];
  currentEndgameScore +=
      multiplier * pestoEndgame[piece_type_moving][flipped_target_sq];

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
      currentHashValue ^= zkeys.enpassentFile[currentEnpassentSquare % 8];
      currentHalfmoveClock = 0;
      break;
    }

    case CAPTURE: {
      currentHalfmoveClock = 0;
      Bitboard capture_mask = (1ULL << target_sq);
      pieceBitboards[piece_getting_captured] ^= capture_mask;
      sideOccupancies[opposite_side] ^= capture_mask;
      currentHashValue ^= zkeys.pieces[piece_getting_captured][target_sq];
      currentMiddlegameScore +=
          multiplier *
          pestoMiddlegame[piece_type_getting_captured][flipped_victim_sq];
      currentEndgameScore +=
          multiplier *
          pestoEndgame[piece_type_getting_captured][flipped_victim_sq];
      currentPhase -= piece_phase[piece_type_getting_captured];
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
        currentHashValue ^= zkeys.pieces[W_ROOK][H1];
        currentHashValue ^= zkeys.pieces[W_ROOK][F1];
      } else {
        castle_mask = (1ULL << H8) | (1ULL << F8);
        pieceBitboards[B_ROOK] ^= castle_mask;
        sideOccupancies[BLACK] ^= castle_mask;
        pieceOnSquare[H8] = NB_PIECES;
        pieceOnSquare[F8] = B_ROOK;
        currentHashValue ^= zkeys.pieces[B_ROOK][H8];
        currentHashValue ^= zkeys.pieces[B_ROOK][F8];
      }

      // flipped H8 is H1, same with other
      currentMiddlegameScore -= multiplier * pestoMiddlegame[ROOK][H1];
      currentMiddlegameScore += multiplier * pestoMiddlegame[ROOK][F1];
      currentEndgameScore -= multiplier * pestoEndgame[ROOK][H1];
      currentEndgameScore += multiplier * pestoEndgame[ROOK][F1];
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
        currentHashValue ^= zkeys.pieces[W_ROOK][A1];
        currentHashValue ^= zkeys.pieces[W_ROOK][D1];
      } else {
        castle_mask = (1ULL << A8) | (1ULL << D8);
        pieceBitboards[B_ROOK] ^= castle_mask;
        sideOccupancies[BLACK] ^= castle_mask;
        pieceOnSquare[A8] = NB_PIECES;
        pieceOnSquare[D8] = B_ROOK;
        currentHashValue ^= zkeys.pieces[B_ROOK][A8];
        currentHashValue ^= zkeys.pieces[B_ROOK][D8];
      }
      currentMiddlegameScore -= multiplier * pestoMiddlegame[ROOK][A1];
      currentMiddlegameScore += multiplier * pestoMiddlegame[ROOK][D1];
      currentEndgameScore -= multiplier * pestoEndgame[ROOK][A1];
      currentEndgameScore += multiplier * pestoEndgame[ROOK][D1];
      break;
    }

    case ENPASS_CAP: {
      currentHalfmoveClock = 0;
      Square flipped_enpass_victim_square;
      if (currentSideToMove == WHITE) {
        Square enpass_cap_sq = static_cast<Square>(target_sq + 8);
        flipped_enpass_victim_square = static_cast<Square>(FLIP(enpass_cap_sq));
        pieceBitboards[B_PAWN] ^= (1ULL << enpass_cap_sq);
        sideOccupancies[BLACK] ^= (1ULL << enpass_cap_sq);
        pieceOnSquare[enpass_cap_sq] = NB_PIECES;
        stateHistory[currentPly].capturedPiece = B_PAWN;
        currentHashValue ^= zkeys.pieces[B_PAWN][enpass_cap_sq];
      } else {
        Square enpass_cap_sq = static_cast<Square>(target_sq - 8);
        flipped_enpass_victim_square = enpass_cap_sq;
        pieceBitboards[W_PAWN] ^= (1ULL << enpass_cap_sq);
        sideOccupancies[WHITE] ^= (1ULL << enpass_cap_sq);
        pieceOnSquare[enpass_cap_sq] = NB_PIECES;
        stateHistory[currentPly].capturedPiece = W_PAWN;
        currentHashValue ^= zkeys.pieces[W_PAWN][enpass_cap_sq];
      }
      currentMiddlegameScore +=
          multiplier * pestoMiddlegame[PAWN][flipped_enpass_victim_square];
      currentEndgameScore +=
          multiplier * pestoEndgame[PAWN][flipped_enpass_victim_square];
      break;
    }

    case QUEEN_PROM: {
      currentHalfmoveClock = 0;
      Bitboard prom_sq_mask = (1ULL << target_sq);

      if (currentSideToMove == WHITE) {
        pieceOnSquare[target_sq] = W_QUEEN;
        pieceBitboards[W_PAWN] ^= prom_sq_mask;
        pieceBitboards[W_QUEEN] ^= prom_sq_mask;
        currentHashValue ^= zkeys.pieces[W_PAWN][target_sq];
        currentHashValue ^= zkeys.pieces[W_QUEEN][target_sq];
      } else {
        pieceOnSquare[target_sq] = B_QUEEN;
        pieceBitboards[B_PAWN] ^= prom_sq_mask;
        pieceBitboards[B_QUEEN] ^= prom_sq_mask;
        currentHashValue ^= zkeys.pieces[B_PAWN][target_sq];
        currentHashValue ^= zkeys.pieces[B_QUEEN][target_sq];
      }
      currentMiddlegameScore -=
          multiplier * pestoMiddlegame[PAWN][flipped_target_sq];
      currentMiddlegameScore +=
          multiplier * pestoMiddlegame[QUEEN][flipped_target_sq];
      currentEndgameScore -= multiplier * pestoEndgame[PAWN][flipped_target_sq];
      currentEndgameScore +=
          multiplier * pestoEndgame[QUEEN][flipped_target_sq];
      currentPhase += piece_phase[QUEEN];
      break;
    }

    case ROOK_PROM: {
      currentHalfmoveClock = 0;
      Bitboard prom_sq_mask = (1ULL << target_sq);

      if (currentSideToMove == WHITE) {
        pieceOnSquare[target_sq] = W_ROOK;
        pieceBitboards[W_PAWN] ^= prom_sq_mask;
        pieceBitboards[W_ROOK] ^= prom_sq_mask;
        currentHashValue ^= zkeys.pieces[W_PAWN][target_sq];
        currentHashValue ^= zkeys.pieces[W_ROOK][target_sq];
      } else {
        pieceOnSquare[target_sq] = B_ROOK;
        pieceBitboards[B_PAWN] ^= prom_sq_mask;
        pieceBitboards[B_ROOK] ^= prom_sq_mask;
        currentHashValue ^= zkeys.pieces[B_PAWN][target_sq];
        currentHashValue ^= zkeys.pieces[B_ROOK][target_sq];
      }
      currentMiddlegameScore -=
          multiplier * pestoMiddlegame[PAWN][flipped_target_sq];
      currentMiddlegameScore +=
          multiplier * pestoMiddlegame[ROOK][flipped_target_sq];
      currentEndgameScore -= multiplier * pestoEndgame[PAWN][flipped_target_sq];
      currentEndgameScore += multiplier * pestoEndgame[ROOK][flipped_target_sq];
      currentPhase += piece_phase[ROOK];
      break;
    }

    case BISHOP_PROM: {
      currentHalfmoveClock = 0;
      Bitboard prom_sq_mask = (1ULL << target_sq);

      if (currentSideToMove == WHITE) {
        pieceOnSquare[target_sq] = W_BISHOP;
        pieceBitboards[W_PAWN] ^= prom_sq_mask;
        pieceBitboards[W_BISHOP] ^= prom_sq_mask;
        currentHashValue ^= zkeys.pieces[W_PAWN][target_sq];
        currentHashValue ^= zkeys.pieces[W_BISHOP][target_sq];
      } else {
        pieceOnSquare[target_sq] = B_BISHOP;
        pieceBitboards[B_PAWN] ^= prom_sq_mask;
        pieceBitboards[B_BISHOP] ^= prom_sq_mask;
        currentHashValue ^= zkeys.pieces[B_PAWN][target_sq];
        currentHashValue ^= zkeys.pieces[B_BISHOP][target_sq];
      }
      currentMiddlegameScore -=
          multiplier * pestoMiddlegame[PAWN][flipped_target_sq];
      currentMiddlegameScore +=
          multiplier * pestoMiddlegame[BISHOP][flipped_target_sq];
      currentEndgameScore -= multiplier * pestoEndgame[PAWN][flipped_target_sq];
      currentEndgameScore +=
          multiplier * pestoEndgame[BISHOP][flipped_target_sq];
      currentPhase += piece_phase[BISHOP];
      break;
    }

    case KNIGHT_PROM: {
      currentHalfmoveClock = 0;
      Bitboard prom_sq_mask = (1ULL << target_sq);

      if (currentSideToMove == WHITE) {
        pieceOnSquare[target_sq] = W_KNIGHT;
        pieceBitboards[W_PAWN] ^= prom_sq_mask;
        pieceBitboards[W_KNIGHT] ^= prom_sq_mask;
        currentHashValue ^= zkeys.pieces[W_PAWN][target_sq];
        currentHashValue ^= zkeys.pieces[W_KNIGHT][target_sq];
      } else {
        pieceOnSquare[target_sq] = B_KNIGHT;
        pieceBitboards[B_PAWN] ^= prom_sq_mask;
        pieceBitboards[B_KNIGHT] ^= prom_sq_mask;
        currentHashValue ^= zkeys.pieces[B_PAWN][target_sq];
        currentHashValue ^= zkeys.pieces[B_KNIGHT][target_sq];
      }
      currentMiddlegameScore -=
          multiplier * pestoMiddlegame[PAWN][flipped_target_sq];
      currentMiddlegameScore +=
          multiplier * pestoMiddlegame[KNIGHT][flipped_target_sq];
      currentEndgameScore -= multiplier * pestoEndgame[PAWN][flipped_target_sq];
      currentEndgameScore +=
          multiplier * pestoEndgame[KNIGHT][flipped_target_sq];
      currentPhase += piece_phase[KNIGHT];
      break;
    }

    case CAP_QUEEN_PROM: {
      currentHalfmoveClock = 0;
      Bitboard prom_sq_mask = (1ULL << target_sq);

      if (currentSideToMove == WHITE) {
        pieceOnSquare[target_sq] = W_QUEEN;
        pieceBitboards[W_PAWN] ^= prom_sq_mask;
        pieceBitboards[W_QUEEN] ^= prom_sq_mask;
        currentHashValue ^= zkeys.pieces[W_PAWN][target_sq];
        currentHashValue ^= zkeys.pieces[W_QUEEN][target_sq];
      } else {
        pieceOnSquare[target_sq] = B_QUEEN;
        pieceBitboards[B_PAWN] ^= prom_sq_mask;
        pieceBitboards[B_QUEEN] ^= prom_sq_mask;
        currentHashValue ^= zkeys.pieces[B_PAWN][target_sq];
        currentHashValue ^= zkeys.pieces[B_QUEEN][target_sq];
      }
      currentMiddlegameScore +=
          multiplier *
          pestoMiddlegame[piece_type_getting_captured][flipped_victim_sq];
      currentMiddlegameScore -=
          multiplier * pestoMiddlegame[PAWN][flipped_target_sq];
      currentMiddlegameScore +=
          multiplier * pestoMiddlegame[QUEEN][flipped_target_sq];
      currentEndgameScore +=
          multiplier *
          pestoEndgame[piece_type_getting_captured][flipped_victim_sq];
      currentEndgameScore -= multiplier * pestoEndgame[PAWN][flipped_target_sq];
      currentEndgameScore +=
          multiplier * pestoEndgame[QUEEN][flipped_target_sq];
      currentPhase -= piece_phase[piece_type_getting_captured];
      currentPhase += piece_phase[QUEEN];

      pieceBitboards[piece_getting_captured] ^= prom_sq_mask;
      sideOccupancies[opposite_side] ^= prom_sq_mask;
      currentHashValue ^= zkeys.pieces[piece_getting_captured][target_sq];
      break;
    }

    case CAP_ROOK_PROM: {
      currentHalfmoveClock = 0;
      Bitboard prom_sq_mask = (1ULL << target_sq);

      if (currentSideToMove == WHITE) {
        pieceOnSquare[target_sq] = W_ROOK;
        pieceBitboards[W_PAWN] ^= prom_sq_mask;
        pieceBitboards[W_ROOK] ^= prom_sq_mask;
        currentHashValue ^= zkeys.pieces[W_PAWN][target_sq];
        currentHashValue ^= zkeys.pieces[W_ROOK][target_sq];
      } else {
        pieceOnSquare[target_sq] = B_ROOK;
        pieceBitboards[B_PAWN] ^= prom_sq_mask;
        pieceBitboards[B_ROOK] ^= prom_sq_mask;
        currentHashValue ^= zkeys.pieces[B_PAWN][target_sq];
        currentHashValue ^= zkeys.pieces[B_ROOK][target_sq];
      }
      currentMiddlegameScore +=
          multiplier *
          pestoMiddlegame[piece_type_getting_captured][flipped_victim_sq];
      currentMiddlegameScore -=
          multiplier * pestoMiddlegame[PAWN][flipped_target_sq];
      currentMiddlegameScore +=
          multiplier * pestoMiddlegame[ROOK][flipped_target_sq];
      currentEndgameScore +=
          multiplier *
          pestoEndgame[piece_type_getting_captured][flipped_victim_sq];
      currentEndgameScore -= multiplier * pestoEndgame[PAWN][flipped_target_sq];
      currentEndgameScore += multiplier * pestoEndgame[ROOK][flipped_target_sq];
      currentPhase -= piece_phase[piece_type_getting_captured];
      currentPhase += piece_phase[ROOK];

      pieceBitboards[piece_getting_captured] ^= prom_sq_mask;
      sideOccupancies[opposite_side] ^= prom_sq_mask;
      currentHashValue ^= zkeys.pieces[piece_getting_captured][target_sq];
      break;
    }

    case CAP_BISHOP_PROM: {
      currentHalfmoveClock = 0;
      Bitboard prom_sq_mask = (1ULL << target_sq);

      if (currentSideToMove == WHITE) {
        pieceOnSquare[target_sq] = W_BISHOP;
        pieceBitboards[W_PAWN] ^= prom_sq_mask;
        pieceBitboards[W_BISHOP] ^= prom_sq_mask;
        currentHashValue ^= zkeys.pieces[W_PAWN][target_sq];
        currentHashValue ^= zkeys.pieces[W_BISHOP][target_sq];
      } else {
        pieceOnSquare[target_sq] = B_BISHOP;
        pieceBitboards[B_PAWN] ^= prom_sq_mask;
        pieceBitboards[B_BISHOP] ^= prom_sq_mask;
        currentHashValue ^= zkeys.pieces[B_PAWN][target_sq];
        currentHashValue ^= zkeys.pieces[B_BISHOP][target_sq];
      }
      currentMiddlegameScore +=
          multiplier *
          pestoMiddlegame[piece_type_getting_captured][flipped_victim_sq];
      currentMiddlegameScore -=
          multiplier * pestoMiddlegame[PAWN][flipped_target_sq];
      currentMiddlegameScore +=
          multiplier * pestoMiddlegame[BISHOP][flipped_target_sq];
      currentEndgameScore +=
          multiplier *
          pestoEndgame[piece_type_getting_captured][flipped_victim_sq];
      currentEndgameScore -= multiplier * pestoEndgame[PAWN][flipped_target_sq];
      currentEndgameScore +=
          multiplier * pestoEndgame[BISHOP][flipped_target_sq];
      currentPhase -= piece_phase[piece_type_getting_captured];
      currentPhase += piece_phase[BISHOP];

      pieceBitboards[piece_getting_captured] ^= prom_sq_mask;
      sideOccupancies[opposite_side] ^= prom_sq_mask;
      currentHashValue ^= zkeys.pieces[piece_getting_captured][target_sq];
      break;
    }

    case CAP_KNIGHT_PROM: {
      currentHalfmoveClock = 0;
      Bitboard prom_sq_mask = (1ULL << target_sq);

      if (currentSideToMove == WHITE) {
        pieceOnSquare[target_sq] = W_KNIGHT;
        pieceBitboards[W_PAWN] ^= prom_sq_mask;
        pieceBitboards[W_KNIGHT] ^= prom_sq_mask;
        currentHashValue ^= zkeys.pieces[W_PAWN][target_sq];
        currentHashValue ^= zkeys.pieces[W_KNIGHT][target_sq];
      } else {
        pieceOnSquare[target_sq] = B_KNIGHT;
        pieceBitboards[B_PAWN] ^= prom_sq_mask;
        pieceBitboards[B_KNIGHT] ^= prom_sq_mask;
        currentHashValue ^= zkeys.pieces[B_PAWN][target_sq];
        currentHashValue ^= zkeys.pieces[B_KNIGHT][target_sq];
      }
      currentMiddlegameScore +=
          multiplier *
          pestoMiddlegame[piece_type_getting_captured][flipped_victim_sq];
      currentMiddlegameScore -=
          multiplier * pestoMiddlegame[PAWN][flipped_target_sq];
      currentMiddlegameScore +=
          multiplier * pestoMiddlegame[KNIGHT][flipped_target_sq];
      currentEndgameScore +=
          multiplier *
          pestoEndgame[piece_type_getting_captured][flipped_victim_sq];
      currentEndgameScore -= multiplier * pestoEndgame[PAWN][flipped_target_sq];
      currentEndgameScore +=
          multiplier * pestoEndgame[KNIGHT][flipped_target_sq];
      currentPhase -= piece_phase[piece_type_getting_captured];
      currentPhase += piece_phase[KNIGHT];

      pieceBitboards[piece_getting_captured] ^= prom_sq_mask;
      sideOccupancies[opposite_side] ^= prom_sq_mask;
      currentHashValue ^= zkeys.pieces[piece_getting_captured][target_sq];
      break;
    }

    default:
      break;
  }

  currentPly++;
  sideOccupancies[NB_COLOR] = sideOccupancies[WHITE] | sideOccupancies[BLACK];
  currentHashValue ^= zkeys.castling[currentCastlingRights];
  currentHashValue ^= zkeys.sideToMove;
  currentSideToMove = (currentSideToMove == WHITE) ? BLACK : WHITE;
}

inline void Board::undo_move(Move mv) {
  currentPly--;
  // currentSide to move is set to the previous side to move
  currentSideToMove = (currentSideToMove == WHITE) ? BLACK : WHITE;
  Color opposite_side = (currentSideToMove == WHITE) ? BLACK : WHITE;

  StateInfo previousState = stateHistory[currentPly];
  Piece captured_piece = previousState.capturedPiece;
  currentEnpassentSquare = previousState.enpassentSquare;
  currentCastlingRights = previousState.castlingRights;
  currentHalfmoveClock = previousState.halfmoveClock;
  currentHashValue = previousState.hashValue;
  currentMiddlegameScore = previousState.middlegameScore;
  currentEndgameScore = previousState.endgameScore;
  currentPhase = previousState.phase;

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
        Square prev_enpass_cap_sq = static_cast<Square>(previous_target_sq + 8);
        pieceBitboards[B_PAWN] ^= (1ULL << prev_enpass_cap_sq);
        sideOccupancies[BLACK] ^= (1ULL << prev_enpass_cap_sq);
        pieceOnSquare[previous_target_sq] = NB_PIECES;
        pieceOnSquare[prev_enpass_cap_sq] = B_PAWN;
      } else {
        Square prev_enpass_cap_sq = static_cast<Square>(previous_target_sq - 8);
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

inline bool Board::is_position_legal() {
  // Since we already made our move, the currentSideToMove will be the
  // opponent
  Color our_side = (currentSideToMove == WHITE) ? BLACK : WHITE;
  Color opp_side = (our_side == WHITE) ? BLACK : WHITE;

  Piece our_king = (our_side == WHITE) ? W_KING : B_KING;
  // Since there is only one king per side, get_lsb_index gets us the location
  // of our king
  Square king_sq = static_cast<Square>(get_lsb_index(pieceBitboards[our_king]));

  // If square in which king is present is attacked then it means our previous
  // move is illegal
  return !(is_square_attacked(king_sq, opp_side));
}

// The idea is that, instead of checking each piece if its attacked or not, we
// will use the idea that for a piece present on square sq1 is attacking a
// particular square sq2 then the same on piece on sq2 attacks sq1, so for
// example, if we want to check if the rook is attacking sq2, we place a rook
// (called attacker) on sq2, get the attacks and & it with rook occupancies,
// if its not 0 then sq2 is attacked by rook
inline bool Board::is_square_attacked(Square sq, Color side) const {
  Piece our_pawn, our_knight, our_bishop, our_rook, our_queen, our_king;

  if (side == WHITE) {
    our_pawn = W_PAWN;
    our_knight = W_KNIGHT;
    our_bishop = W_BISHOP;
    our_rook = W_ROOK;
    our_queen = W_QUEEN;
    our_king = W_KING;
  } else {
    our_pawn = B_PAWN;
    our_knight = B_KNIGHT;
    our_bishop = B_BISHOP;
    our_rook = B_ROOK;
    our_queen = B_QUEEN;
    our_king = B_KING;
  }

  Bitboard attacker_pawn_bb, attacker_knight_bb, attacker_bishop_bb,
      attacker_rook_bb, attacker_king_bb;

  Color opposite_side = (side == WHITE ? BLACK : WHITE);
  attacker_pawn_bb = kPawnAttacks[opposite_side][sq];
  if ((attacker_pawn_bb & pieceBitboards[our_pawn])) return true;

  attacker_knight_bb = kKnightAttacks[sq];
  if (attacker_knight_bb & pieceBitboards[our_knight]) return true;

  attacker_king_bb = kKingAttacks[sq];
  if (attacker_king_bb & pieceBitboards[our_king]) return true;

  attacker_bishop_bb = BishopAttacks(sideOccupancies[NB_COLOR], sq);
  if (attacker_bishop_bb &
      (pieceBitboards[our_bishop] | pieceBitboards[our_queen]))
    return true;

  attacker_rook_bb = RookAttacks(sideOccupancies[NB_COLOR], sq);
  if (attacker_rook_bb & (pieceBitboards[our_rook] | pieceBitboards[our_queen]))
    return true;

  return false;
}

inline Bitboard Board::get_piece_bitboard(Piece p) const {
  return pieceBitboards[p];
}

inline Piece Board::get_piece_at_square(Square sq) const {
  return pieceOnSquare[sq];
}

inline Color Board::get_side_to_move() const { return currentSideToMove; }

inline Bitboard Board::get_side_occupancy(Color side) const {
  return sideOccupancies[side];
}

inline Square Board::get_enpassent_sq() const { return currentEnpassentSquare; }

inline uint8_t Board::get_castling_rights() const {
  return currentCastlingRights;
}

inline uint64_t Board::get_current_hash() const { return currentHashValue; }

inline short int Board::get_current_middlegame_score() const {
  return currentMiddlegameScore;
}

inline short int Board::get_current_endgame_score() const {
  return currentEndgameScore;
}

inline uint8_t Board::get_current_phase() const { return currentPhase; }

Board::Board() {
  StateInfo emptyInfo = {NB_PIECES, NB_SQ, 0, 0, 0, 0, 0, 0ULL};
  stateHistory.fill(emptyInfo);
  pieceBitboards.fill(0ULL);
  sideOccupancies.fill(0ULL);
  pieceOnSquare.fill(NB_PIECES);

  currentSideToMove = NB_COLOR;
  pieceCapturedPreviously = NB_PIECES;
  currentEnpassentSquare = NB_SQ;
  currentCastlingRights = 0x0F;
  currentHalfmoveClock = 0;
  currentHashValue = 0ULL;
}
