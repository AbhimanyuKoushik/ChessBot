#include <board.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <types.hpp>

uint64_t Board::calculateHashFromScratch() {
  uint64_t hash = 0ULL;

  for (int square = 0; square < 64; square++) {
    Piece piece = pieceOnSquare[square];
    if (piece != NB_PIECES) hash ^= zkeys.pieces[piece][square];
  }

  hash ^= zkeys.castling[currentCastlingRights];

  if (currentEnpassentSquare != NB_SQ) {
    hash ^= zkeys.enpassentFile[currentEnpassentSquare % 8];
  }

  if (currentSideToMove == BLACK) {
    hash ^= zkeys.sideToMove;
  }

  return hash;
}

void Board::InitializeBoard() {
  pieceBitboards[W_PAWN] = 0x00'FF'00'00'00'00'00'00ULL;
  pieceBitboards[W_KNIGHT] = 0x42'00'00'00'00'00'00'00ULL;
  pieceBitboards[W_BISHOP] = 0x24'00'00'00'00'00'00'00ULL;
  pieceBitboards[W_ROOK] = 0x81'00'00'00'00'00'00'00ULL;
  pieceBitboards[W_QUEEN] = 0x08'00'00'00'00'00'00'00ULL;
  pieceBitboards[W_KING] = 0x10'00'00'00'00'00'00'00ULL;

  pieceBitboards[B_PAWN] = 0x00'00'00'00'00'00'FF'00ULL;
  pieceBitboards[B_KNIGHT] = 0x00'00'00'00'00'00'00'42ULL;
  pieceBitboards[B_BISHOP] = 0x00'00'00'00'00'00'00'24ULL;
  pieceBitboards[B_ROOK] = 0x00'00'00'00'00'00'00'81ULL;
  pieceBitboards[B_QUEEN] = 0x00'00'00'00'00'00'00'08ULL;
  pieceBitboards[B_KING] = 0x00'00'00'00'00'00'00'10ULL;

  sideOccupancies[WHITE] = 0xFF'FF'00'00'00'00'00'00ULL;
  sideOccupancies[BLACK] = 0x00'00'00'00'00'00'FF'FFULL;
  sideOccupancies[NB_COLOR] = 0xFF'FF'00'00'00'00'FF'FFULL;

  currentCastlingRights = 0x0F;
  currentEnpassentSquare = NB_SQ;
  currentHalfmoveClock = 0;
  currentPly = 0;
  pieceCapturedPreviously = NB_PIECES;
  currentSideToMove = WHITE;

  // clang-format off
  pieceOnSquare[A8] = B_ROOK;    pieceOnSquare[B8] = B_KNIGHT;    pieceOnSquare[C8] = B_BISHOP;    pieceOnSquare[D8] = B_QUEEN;     pieceOnSquare[E8] = B_KING;    pieceOnSquare[F8] = B_BISHOP;    pieceOnSquare[G8] = B_KNIGHT;    pieceOnSquare[H8] = B_ROOK; 
  pieceOnSquare[A7] = B_PAWN;    pieceOnSquare[B7] = B_PAWN;      pieceOnSquare[C7] = B_PAWN;      pieceOnSquare[D7] = B_PAWN;      pieceOnSquare[E7] = B_PAWN;    pieceOnSquare[F7] = B_PAWN;      pieceOnSquare[G7] = B_PAWN;      pieceOnSquare[H7] = B_PAWN; 
  pieceOnSquare[A6] = NB_PIECES; pieceOnSquare[B6] = NB_PIECES;   pieceOnSquare[C6] = NB_PIECES;   pieceOnSquare[D6] = NB_PIECES;   pieceOnSquare[E6] = NB_PIECES; pieceOnSquare[F6] = NB_PIECES;   pieceOnSquare[G6] = NB_PIECES;   pieceOnSquare[H6] = NB_PIECES; 
  pieceOnSquare[A5] = NB_PIECES; pieceOnSquare[B5] = NB_PIECES;   pieceOnSquare[C5] = NB_PIECES;   pieceOnSquare[D5] = NB_PIECES;   pieceOnSquare[E5] = NB_PIECES; pieceOnSquare[F5] = NB_PIECES;   pieceOnSquare[G5] = NB_PIECES;   pieceOnSquare[H5] = NB_PIECES; 
  pieceOnSquare[A4] = NB_PIECES; pieceOnSquare[B4] = NB_PIECES;   pieceOnSquare[C4] = NB_PIECES;   pieceOnSquare[D4] = NB_PIECES;   pieceOnSquare[E4] = NB_PIECES; pieceOnSquare[F4] = NB_PIECES;   pieceOnSquare[G4] = NB_PIECES;   pieceOnSquare[H4] = NB_PIECES; 
  pieceOnSquare[A3] = NB_PIECES; pieceOnSquare[B3] = NB_PIECES;   pieceOnSquare[C3] = NB_PIECES;   pieceOnSquare[D3] = NB_PIECES;   pieceOnSquare[E3] = NB_PIECES; pieceOnSquare[F3] = NB_PIECES;   pieceOnSquare[G3] = NB_PIECES;   pieceOnSquare[H3] = NB_PIECES; 
  pieceOnSquare[A2] = W_PAWN;    pieceOnSquare[B2] = W_PAWN;      pieceOnSquare[C2] = W_PAWN;      pieceOnSquare[D2] = W_PAWN;      pieceOnSquare[E2] = W_PAWN;    pieceOnSquare[F2] = W_PAWN;      pieceOnSquare[G2] = W_PAWN;      pieceOnSquare[H2] = W_PAWN; 
  pieceOnSquare[A1] = W_ROOK;    pieceOnSquare[B1] = W_KNIGHT;    pieceOnSquare[C1] = W_BISHOP;    pieceOnSquare[D1] = W_QUEEN;     pieceOnSquare[E1] = W_KING;    pieceOnSquare[F1] = W_BISHOP;    pieceOnSquare[G1] = W_KNIGHT;    pieceOnSquare[H1] = W_ROOK;
  // clang-format on

  currentHashValue = calculateHashFromScratch();
  StateInfo emptyInfo = {NB_PIECES, NB_SQ, 0, 0, 0ULL};
  stateHistory.fill(emptyInfo);
}

// clang-format off
inline constexpr std::array<const char*, 12> unicode_pieces = {
    "♟︎", "♞", "♝", "♜", "♛", "♚",  // White
    "♙", "♘", "♗", "♖", "♕", "♔"   // Black
};
// clang-format on

void Board::printBoard() {
  std::cout << "\n";

  // 1) Board diagram: ranks 8 down to 1
  for (int rank = 7; rank >= 0; --rank) {
    std::cout << (rank + 1) << " ";
    for (int file = 0; file < 8; ++file) {
      // Top-Down indexing: A8 is 0, H1 is 63
      int sq = (7 - rank) * 8 + file;
      Piece p = pieceOnSquare[sq];

      if (p == NB_PIECES) {
        std::cout << ". ";
      } else {
        std::cout << unicode_pieces[p] << " ";
      }
    }
    std::cout << '\n';
  }

  // 2) File labels
  std::cout << "  a b c d e f g h\n\n";

  // 3) Side to move
  std::cout << "Side to move: "
            << (currentSideToMove == WHITE ? "White" : "Black") << '\n';

  // 4) En passant
  std::cout << "Enpassant Square: ";
  if (currentEnpassentSquare == NB_SQ) {
    std::cout << "None\n";
  } else {
    // Convert 0-63 index to algebraic coordinates
    char f = 'a' + (currentEnpassentSquare % 8);
    char r = '8' - (currentEnpassentSquare / 8);
    std::cout << f << r << '\n';
  }

  // 5) Castling rights (Using the 4-bit mask: WK=1, WQ=2, BK=4, BQ=8)
  std::cout << "Castling: ";
  std::cout << ((currentCastlingRights & 1) ? "K" : "-");
  std::cout << ((currentCastlingRights & 2) ? "Q" : "-");
  std::cout << ((currentCastlingRights & 4) ? "k" : "-");
  std::cout << ((currentCastlingRights & 8) ? "q" : "-");
  std::cout << "\n";
  std::cout << "The position is " << ((is_position_legal()) ? "" : "not ")
            << "legal.\n\n";
}

void Board::LoadFEN(const std::string& fenString) {
  std::istringstream stream(fenString);
  std::string boardPart, turnPart, castlingRightsPart, enpassentSquarePart,
      halfMoveClockPart;

  stream >> boardPart >> turnPart >> castlingRightsPart >> enpassentSquarePart;

  if (!(stream >> halfMoveClockPart)) halfMoveClockPart = "0";

  if (enpassentSquarePart == "-") {
    currentEnpassentSquare = NB_SQ;
  } else {
    // Convert algebraic (e.g., "e3") to square index (Assuming Top-Down A8=0)
    int file = enpassentSquarePart[0] - 'a';
    int rank = 7 - (enpassentSquarePart[1] - '1');
    currentEnpassentSquare = static_cast<Square>(rank * 8 + file);
  }

  if (halfMoveClockPart == "-")
    currentHalfmoveClock = 0;
  else
    currentHalfmoveClock = std::stoi(halfMoveClockPart);

  currentCastlingRights = 0;

  if (castlingRightsPart.find('K') != std::string::npos)
    currentCastlingRights |= 0x01;
  if (castlingRightsPart.find('Q') != std::string::npos)
    currentCastlingRights |= 0x02;
  if (castlingRightsPart.find('k') != std::string::npos)
    currentCastlingRights |= 0x04;
  if (castlingRightsPart.find('q') != std::string::npos)
    currentCastlingRights |= 0x08;

  pieceCapturedPreviously = NB_PIECES;
  currentPly = 0;
  currentSideToMove = (turnPart == "w") ? WHITE : BLACK;

  for (Piece piece = W_PAWN; piece <= B_KING;
       piece = static_cast<Piece>(piece + 1)) {
    pieceBitboards[piece] = 0ULL;
  }
  pieceOnSquare.fill(NB_PIECES);

  Square sq = static_cast<Square>(0);
  Piece currentPiece;

  for (char character : boardPart) {
    if (std::isdigit(character)) {
      sq = static_cast<Square>(sq + (character - '0'));
    } else if (character == '/') {
      continue;
    } else {
      switch (character) {
        case 'P':
          currentPiece = W_PAWN;
          break;
        case 'N':
          currentPiece = W_KNIGHT;
          break;
        case 'B':
          currentPiece = W_BISHOP;
          break;
        case 'R':
          currentPiece = W_ROOK;
          break;
        case 'Q':
          currentPiece = W_QUEEN;
          break;
        case 'K':
          currentPiece = W_KING;
          break;
        case 'p':
          currentPiece = B_PAWN;
          break;
        case 'n':
          currentPiece = B_KNIGHT;
          break;
        case 'b':
          currentPiece = B_BISHOP;
          break;
        case 'r':
          currentPiece = B_ROOK;
          break;
        case 'q':
          currentPiece = B_QUEEN;
          break;
        case 'k':
          currentPiece = B_KING;
          break;
        default:
          currentPiece = NB_PIECES;
          break;
      }

      if (currentPiece != NB_PIECES) {
        set_bit(pieceBitboards[currentPiece], sq);
        pieceOnSquare[sq] = currentPiece;
        sq = static_cast<Square>(sq + 1);
      }
    }
  }

  // Update occupancies and hashValue
  currentHashValue = calculateHashFromScratch();
  sideOccupancies[WHITE] = pieceBitboards[W_PAWN] | pieceBitboards[W_KNIGHT] |
                           pieceBitboards[W_BISHOP] | pieceBitboards[W_ROOK] |
                           pieceBitboards[W_QUEEN] | pieceBitboards[W_KING];

  sideOccupancies[BLACK] = pieceBitboards[B_PAWN] | pieceBitboards[B_KNIGHT] |
                           pieceBitboards[B_BISHOP] | pieceBitboards[B_ROOK] |
                           pieceBitboards[B_QUEEN] | pieceBitboards[B_KING];

  sideOccupancies[NB_COLOR] = sideOccupancies[WHITE] | sideOccupancies[BLACK];
}