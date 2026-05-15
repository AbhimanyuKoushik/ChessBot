#include <iostream>
#include <test_utils.hpp>

int main(int argc, char* argv[]) {
  // If no strings are passed, we run everything
  std::string target = (argc > 1) ? argv[1] : "all";
  int failed_tests = 0;

  // Test for bitboard utilities
  if (target == "all" || target == "bitboard") {
    if (!test_bitboard()) {
      std::cout << "[FAIL] Bitboard!\n";
      failed_tests++;
    }
  }

  // Test for pawn, knight and king attack generation
  if (target == "all" || target == "leaper_attack_gen") {
    if (!test_leaper_attack_gen()) {
      std::cout << "[FAIL] Leaper Attack Generation!\n";
      failed_tests++;
    }
  }

  if (failed_tests == 0)
    return 0;
  else
    return 1;
}