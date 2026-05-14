#include <iostream>
#include <test_utils.hpp>

int main(int argc, char* argv[]) {
  // If no strings are passed, we run everything
  std::string target = (argc > 1) ? argv[1] : "all";
  int failed_tests = 0;

  // Test for pawns
  if (target == "all" || target == "pawns") {
    if (!test_pawn_generation()) {
      std::cout << "[FAIL] Pawn Generation!\n";
      failed_tests++;
    }
  }

  // Test for knight
  if (target == "all" || target == "knights") {
    if (!test_knight_generation()) {
      std::cout << "[FAIL] Knight Generation!\n";
      failed_tests++;
    }
  }

  if (failed_tests == 0)
    return 0;
  else
    return 1;
}