#include <board.hpp>
#include <iostream>
#include <types.hpp>
#include <vector>

inline void test_after_make_move(Board& board, Move mv, int ply) {
  std::cout << "\n====================================\n";
  std::cout << "Making Move (Ply " << ply << ")\n";
  std::cout << "====================================\n";
  board.make_move(mv);
  board.printBoard();
}

inline void test_after_undo_move(Board& board, Move mv, int ply) {
  std::cout << "\n====================================\n";
  std::cout << "Undoing Move (Reverting to Ply " << ply << ")\n";
  std::cout << "====================================\n";
  board.undo_move(mv);
  board.printBoard();
}

void test_board() {
  Board board;
  board.InitializeBoard();

  std::cout << "INITIAL BOARD STATE:\n";
  board.printBoard();

  // The Engine Stress Test (18 Plies)
  std::vector<Move> game_moves = {
      encode_move(E2, E4, DOUBLE_PUSH),     // 1. e4
      encode_move(E7, E5, DOUBLE_PUSH),     // 1... e5
      encode_move(D2, D4, DOUBLE_PUSH),     // 2. d4
      encode_move(E5, D4, CAPTURE),         // 2... exd4
      encode_move(C2, C4, DOUBLE_PUSH),     // 3. c4
      encode_move(D4, C3, ENPASS_CAP),      // 3... dxc3 e.p. (En Passant!)
      encode_move(G1, F3, QUIET),           // 4. Nf3
      encode_move(C3, B2, CAPTURE),         // 4... cxb2
      encode_move(F1, C4, QUIET),           // 5. Bc4
      encode_move(B2, C1, CAP_QUEEN_PROM),  // 5... bxc1=Q (Capture & Promote!)
      encode_move(E1, G1, KING_CASTLE),     // 6. O-O (White Kingside)
      encode_move(D7, D6, QUIET),           // 6... d6
      encode_move(B1, C3, QUIET),           // 7. Nc3
      encode_move(C8, G4, QUIET),           // 7... Bg4
      encode_move(D1, D5, QUIET),           // 8. Qd5
      encode_move(D8, D7, QUIET),           // 8... Qd7
      encode_move(D5, B7, CAPTURE),         // 9. Qxb7
      encode_move(E8, C8, QUEEN_CASTLE)     // 9... O-O-O (Black Queenside)
  };

  // 1. PLAY THE GAME FORWARD
  for (size_t i = 0; i < game_moves.size(); ++i) {
    test_after_make_move(board, game_moves[i], i + 1);
  }

  std::cout << "\n\n*** GAME FINISHED. COMMENCING REWIND. ***\n\n";

  // 2. REWIND THE GAME BACKWARDS
  // We iterate backwards from the last move down to the first
  for (int i = game_moves.size() - 1; i >= 0; --i) {
    test_after_undo_move(board, game_moves[i], i);
  }

  std::cout << "\n*** REWIND COMPLETE ***\n";
  std::cout << "The board above should look EXACTLY like the initial starting "
               "position.\n";
}