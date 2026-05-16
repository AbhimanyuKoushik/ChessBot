#include <bitboards.hpp>
#include <iostream>
#include <sliders.hpp>
#include <types.hpp>

bool check_rook_occupancy_mask_gen() {
  Bitboard occupancy_mask;

  // Checking occupancy masks only
  occupancy_mask = kRookOccupancyTable[D4];
  if (occupancy_mask != 2261102847592448ULL) {
    std::cout << "Occupancy mask of Rook on D4 failed.\n";
    print_bitboard(occupancy_mask);
    return false;
  }

  occupancy_mask = kRookOccupancyTable[A1];
  if (occupancy_mask != 9079539427579068672ULL) {
    std::cout << "Occupancy mask of Rook on A1 failed.\n";
    print_bitboard(occupancy_mask);
    return false;
  }

  occupancy_mask = kRookOccupancyTable[H8];
  if (occupancy_mask != 36170086419038334ULL) {
    std::cout << "Occupancy mask of Rook on H8 failed.\n";
    print_bitboard(occupancy_mask);
    return false;
  }

  return true;
}

bool check_bishop_occupancy_mask_gen() {
  Bitboard occupancy_mask;

  occupancy_mask = kBishopOccupancyTable[D4];
  if (occupancy_mask != 9592139778506752ULL) {
    std::cout << "Occupancy mask of Bishop on D4 failed.\n";
    print_bitboard(occupancy_mask);
    return false;
  }

  occupancy_mask = kBishopOccupancyTable[A1];
  if (occupancy_mask != 567382630219776ULL) {
    std::cout << "Occupancy mask of Bishop on A1 failed.\n";
    print_bitboard(occupancy_mask);
    return false;
  }

  occupancy_mask = kBishopOccupancyTable[E4];
  if (occupancy_mask != 19184279556981248ULL) {
    std::cout << "Occupancy mask of Bishop on E4 failed.\n";
    print_bitboard(occupancy_mask);
    return false;
  }

  occupancy_mask = kBishopOccupancyTable[H1];
  if (occupancy_mask != 18049651735527936ULL) {
    std::cout << "Occupancy mask of Bishop on H1 failed.\n";
    print_bitboard(occupancy_mask);
    return false;
  }

  return true;
}

bool check_rook_attack_gen() {
  Bitboard occupied_squares = 0ULL;
  Bitboard attack_mask;

  set_bit(occupied_squares, D7);
  set_bit(occupied_squares, D6);
  set_bit(occupied_squares, D2);
  set_bit(occupied_squares, G4);
  set_bit(occupied_squares, B4);

  attack_mask = rook_attack_gen(occupied_squares, D4);
  if (attack_mask != 2261102847590400ULL) {
    std::cout << "Attack mask generation for rook on D4 with pieces on D7, D6, "
                 "D2, G4, B4 failed.\n";
    print_bitboard(attack_mask);
    return false;
  }

  occupied_squares = 0ULL;
  set_bit(occupied_squares, A8);
  set_bit(occupied_squares, F1);
  set_bit(occupied_squares, D4);
  set_bit(occupied_squares, A7);
  set_bit(occupied_squares, F3);

  attack_mask = rook_attack_gen(occupied_squares, A1);
  if (attack_mask != 4467853409151680768ULL) {
    std::cout << "Attack mask generation for rook on A1 with pieces on A8, F1, "
                 "D4, A7, F3 failed.\n";
    print_bitboard(attack_mask);
    return false;
  }

  return true;
}

bool check_bishop_attack_gen() {
  Bitboard occupied_squares = 0ULL;
  Bitboard attack_mask;

  set_bit(occupied_squares, F6);
  set_bit(occupied_squares, B6);
  set_bit(occupied_squares, A1);
  set_bit(occupied_squares, G7);
  set_bit(occupied_squares, F2);

  attack_mask = bishop_attack_gen(occupied_squares, D4);
  if (attack_mask != 81649733816418304ULL) {
    std::cout << "Attack mask generation for bishop on D4 with pieces on F6, "
                 "B6, A1, G7, F2 failed.\n";
    print_bitboard(attack_mask);
    return false;
  }

  occupied_squares = 0ULL;
  set_bit(occupied_squares, B2);
  set_bit(occupied_squares, G7);
  set_bit(occupied_squares, C2);
  set_bit(occupied_squares, A3);

  attack_mask = bishop_attack_gen(occupied_squares, A1);
  if (attack_mask != 562949953421312ULL) {
    std::cout << "Attack mask generation for bishop on D4 with pieces on B2, "
                 "G7, C2, A3 failed.\n";
    print_bitboard(attack_mask);
    return false;
  }

  return true;
}

bool check_rook_magic_table() {
  Bitboard occupied_squares = 0ULL;
  Bitboard attack_mask;

  set_bit(occupied_squares, D7);
  set_bit(occupied_squares, D6);
  set_bit(occupied_squares, D2);
  set_bit(occupied_squares, G4);
  set_bit(occupied_squares, B4);

  attack_mask = RookAttacks(occupied_squares, D4);
  if (attack_mask != 2261102847590400ULL) {
    std::cout << "Attack mask generation for rook on D4 with pieces on D7, D6, "
                 "D2, G4, B4 failed.\n";
    print_bitboard(attack_mask);
    return false;
  }

  occupied_squares = 0ULL;
  set_bit(occupied_squares, A8);
  set_bit(occupied_squares, F1);
  set_bit(occupied_squares, D4);
  set_bit(occupied_squares, A7);
  set_bit(occupied_squares, F3);

  attack_mask = RookAttacks(occupied_squares, A1);
  if (attack_mask != 4467853409151680768ULL) {
    std::cout << "Attack mask generation for rook on A1 with pieces on A8, F1, "
                 "D4, A7, F3 failed.\n";
    print_bitboard(attack_mask);
    return false;
  }

  return true;
}

bool check_bishop_magic_table() {
  Bitboard occupied_squares = 0ULL;
  Bitboard attack_mask;

  set_bit(occupied_squares, F6);
  set_bit(occupied_squares, B6);
  set_bit(occupied_squares, A1);
  set_bit(occupied_squares, G7);
  set_bit(occupied_squares, F2);

  attack_mask = BishopAttacks(occupied_squares, D4);
  if (attack_mask != 81649733816418304ULL) {
    std::cout << "Attack mask generation for bishop on D4 with pieces on F6, "
                 "B6, A1, G7, F2 failed.\n";
    print_bitboard(attack_mask);
    return false;
  }

  occupied_squares = 0ULL;
  set_bit(occupied_squares, B2);
  set_bit(occupied_squares, G7);
  set_bit(occupied_squares, C2);
  set_bit(occupied_squares, A3);

  attack_mask = BishopAttacks(occupied_squares, A1);
  if (attack_mask != 562949953421312ULL) {
    std::cout << "Attack mask generation for bishop on D4 with pieces on B2, "
                 "G7, C2, A3 failed.\n";
    print_bitboard(attack_mask);
    return false;
  }

  return true;
}

bool test_slider_attack_gen() {
  bool all_passed = check_rook_occupancy_mask_gen();
  all_passed &= check_bishop_occupancy_mask_gen();
  all_passed &= check_rook_attack_gen();
  all_passed &= check_bishop_attack_gen();
  all_passed &= check_rook_magic_table();
  all_passed &= check_bishop_magic_table();
  return all_passed;
}