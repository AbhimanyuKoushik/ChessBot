#include <iostream>
#include <search.hpp>
#include <vector>

struct SearchTest {
  std::string fen_string;
  int depth;
  std::string expected_move;  // in "e1e8" format
};

std::string move_to_string(Move move) {
  int source_square = get_move_source_sq(move);
  int target_square = get_move_target_sq(move);

  std::string move_string;
  move_string += static_cast<char>('a' + (source_square % 8));
  move_string += static_cast<char>('1' + (7 - (source_square / 8)));
  move_string += static_cast<char>('a' + (target_square % 8));
  move_string += static_cast<char>('1' + (7 - (target_square / 8)));

  return move_string;
}

bool test_search() {
  Board board;
  SearchInfo info;
  info.stopped = false;

  std::vector<SearchTest> tests = {
      // Test 1: Mate in 2 (White to move)
      {"q5nr/P7/Bpk2b2/1Np2Qp1/8/4P3/1PP2PKp/R1Br4 w - - 0 31", 4, "f5e6"},

      // Test 2: Free Queen (Black to move)
      // White Queen is hanging on g7, Black King is on f8.
      {"5k2/6Q1/8/8/8/8/8/7K b - - 0 1", 3, "f8g7"},

      // Test 3: Mate in 2 (White to move - Back Rank Mate)
      // White Rook on a1, Black King on g8 trapped by pawns.
      {"6k1/5ppp/8/8/8/8/8/R5K1 w - - 0 1", 4, "a1a8"},

      {"3rrk2/pbq3pQ/1p2p2p/5B2/8/1P6/PBP3PP/3RbK2 w - - 2 23", 4, "h7h8"}};

  for (size_t i = 0; i < tests.size(); i++) {
    board.LoadFEN(tests[i].fen_string);
    info.nodes = 0;

    Move best_move = search_position(board, info, tests[i].depth);

    std::string best_move_string = move_to_string(best_move);

    if (best_move_string != tests[i].expected_move) {
      board.printBoard();
      std::cout << "[FAIL] Test " << i + 1 << ". Expected "
                << tests[i].expected_move << ". Got " << best_move_string
                << ".\n";
      return false;
    }
  }

  return true;
}