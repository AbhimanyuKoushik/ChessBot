#include <bitboards.hpp>
#include <iostream>
#include <sliders.hpp>
#include <types.hpp>

bool test_slider_attack_gen() {
  Bitboard occupancy_mask;

  // Checking occupancy masks only
  occupancy_mask = rook_occupancy_mask(D4);
  if (occupancy_mask != 2261102847592448ULL) {
    std::cout << "Occupancy mask of Rook on D4 failed.\n";
    print_bitboard(occupancy_mask);
    return false;
  }

  occupancy_mask = rook_occupancy_mask(A1);
  if (occupancy_mask != 9079539427579068672ULL) {
    std::cout << "Occupancy mask of Rook on A1 failed.\n";
    print_bitboard(occupancy_mask);
    return false;
  }

  occupancy_mask = rook_occupancy_mask(H8);
  if (occupancy_mask != 36170086419038334ULL) {
    std::cout << "Occupancy mask of Rook on H8 failed.\n";
    print_bitboard(occupancy_mask);
    return false;
  }

  occupancy_mask = bishop_occupancy_mask(D4);
  if (occupancy_mask != 9592139778506752ULL) {
    std::cout << "Occupancy mask of Bishop on D4 failed.\n";
    print_bitboard(occupancy_mask);
    return false;
  }

  occupancy_mask = bishop_occupancy_mask(A1);
  if (occupancy_mask != 567382630219776ULL) {
    std::cout << "Occupancy mask of Bishop on A1 failed.\n";
    print_bitboard(occupancy_mask);
    return false;
  }

  occupancy_mask = bishop_occupancy_mask(E4);
  if (occupancy_mask != 19184279556981248ULL) {
    std::cout << "Occupancy mask of Bishop on E4 failed.\n";
    print_bitboard(occupancy_mask);
    return false;
  }

  occupancy_mask = bishop_occupancy_mask(H1);
  if (occupancy_mask != 18049651735527936ULL) {
    std::cout << "Occupancy mask of Bishop on H1 failed.\n";
    print_bitboard(occupancy_mask);
    return false;
  }
  return true;
}