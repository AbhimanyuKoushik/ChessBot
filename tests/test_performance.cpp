#include <board.hpp>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <movegen.hpp>
#include <string>
#include <utility>  // For std::pair
#include <vector>

// Recursive function to count nodes
inline uint64_t perft(Board& board, int depth) {
  if (depth == 0) {
    return 1ULL;
  }

  MoveList movelist;
  movelist.count = 0;  // Ensure it starts at 0

  // ~0ULL means all 64 squares are valid targets (no mask filter)
  generate_pseudo_legal_moves(board, movelist, ~0ULL);

  uint64_t nodes = 0;

  for (int i = 0; i < movelist.count; i++) {
    Move mv = movelist.moves[i];

    board.make_move(mv);

    // Lazy validation: if the move leaves our king in check, it's illegal
    if (!board.is_position_legal()) {
      board.undo_move(mv);
      continue;
    }

    nodes += perft(board, depth - 1);

    board.undo_move(mv);
  }

  return nodes;
}

// Now returns a pair: {average_nps, all_passed}
inline std::pair<double, bool> test_perf(
    const std::string& fen,
    const std::vector<std::pair<int, uint64_t>>& expected_results) {
  Board board;
  board.LoadFEN(fen);

  std::cout << "\n=== PERFT TESTS FOR: [" << fen << "] ===\n";

  double total_nps = 0.0;
  bool all_passed = true;

  for (const auto& [depth, expected] : expected_results) {
    auto t0 = std::chrono::high_resolution_clock::now();

    uint64_t nodes = perft(board, depth);

    auto t1 = std::chrono::high_resolution_clock::now();
    auto microseconds =
        std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    double ms = microseconds / 1000.0;

    double nps = (microseconds > 0) ? (nodes * 1000000.0 / microseconds) : 0.0;
    total_nps += nps;

    std::cout << "Perft(" << depth << ") = " << nodes;

    if (nodes == expected) {
      std::cout << "  [✓ PASS]";
    } else {
      std::cout << "  [✗ FAIL] (Expected: " << expected << ")";
      all_passed = false;  // Flag failure
    }

    std::cout << "  [" << std::fixed << std::setprecision(2) << ms << " ms, "
              << std::scientific << std::setprecision(2) << nps << " nps]\n";
  }

  double average_nps = total_nps / expected_results.size();
  std::cout << std::defaultfloat;

  return {average_nps, all_passed};
}

bool test_performance() {
  double overall_total_nps = 0.0;
  int positions_tested = 0;

  // This will flip to false if any single depth on any position fails
  bool overall_success = true;

  // --- Position 1: Standard Start ---
  std::string pos1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  std::vector<std::pair<int, uint64_t>> pos1_results = {
      {3, 8902ULL}, {4, 197281ULL}, {5, 4865609ULL}, {6, 119060324ULL}};

  auto [nps1, passed1] = test_perf(pos1, pos1_results);
  overall_total_nps += nps1;
  if (!passed1) overall_success = false;
  positions_tested++;

  // --- Position 2: Kiwipete ---
  std::string pos2 =
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
  std::vector<std::pair<int, uint64_t>> pos2_results = {
      {3, 97862ULL}, {4, 4085603ULL}, {5, 193690690ULL}};

  auto [nps2, passed2] = test_perf(pos2, pos2_results);
  overall_total_nps += nps2;
  if (!passed2) overall_success = false;
  positions_tested++;

  // --- Position 3 ---
  std::string pos3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1 ";
  std::vector<std::pair<int, uint64_t>> pos3_results = {{3, 2812ULL},
                                                        {4, 43238ULL},
                                                        {5, 674624ULL},
                                                        {6, 11030083ULL},
                                                        {7, 178633661ULL}};

  auto [nps3, passed3] = test_perf(pos3, pos3_results);
  overall_total_nps += nps3;
  if (!passed3) overall_success = false;
  positions_tested++;

  // --- Position 4 ---
  std::string pos4 =
      "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
  std::vector<std::pair<int, uint64_t>> pos4_results = {
      {3, 9467ULL}, {4, 422333ULL}, {5, 15833292ULL}, {6, 706045033ULL}};

  auto [nps4, passed4] = test_perf(pos4, pos4_results);
  overall_total_nps += nps4;
  if (!passed4) overall_success = false;
  positions_tested++;

  std::string pos5 =
      "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ";
  std::vector<std::pair<int, uint64_t>> pos5_results = {
      {3, 62379ULL}, {4, 2103487ULL}, {5, 89941194ULL}};

  auto [nps5, passed5] = test_perf(pos5, pos5_results);
  overall_total_nps += nps5;
  if (!passed5) overall_success = false;
  positions_tested++;

  std::string pos6 =
      "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 "
      "10 ";
  std::vector<std::pair<int, uint64_t>> pos6_results = {
      {3, 89890ULL}, {4, 3894594ULL}, {5, 164075551ULL}};

  auto [nps6, passed6] = test_perf(pos6, pos6_results);
  overall_total_nps += nps6;
  if (!passed6) overall_success = false;
  positions_tested++;

  // --- Final Calculation ---
  double engine_speed = overall_total_nps / positions_tested;

  std::cout << "\n========================================\n";
  std::cout << "OVERALL ENGINE SPEED: " << std::scientific
            << std::setprecision(2) << engine_speed << " nps\n";

  // Return the strict boolean evaluation of the entire test suite
  return overall_success;
}