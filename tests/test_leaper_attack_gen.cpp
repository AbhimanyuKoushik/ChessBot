#include <bitboards.hpp>
#include <iostream>
#include <leapers.hpp>
#include <types.hpp>

bool test_leaper_attack_gen() {
  Bitboard white_pawn_attack, black_pawn_attack, knight_attack, king_attack;

  white_pawn_attack = kPawnAttacks[WHITE][D4];
  black_pawn_attack = kPawnAttacks[BLACK][D4];
  knight_attack = kKnightAttacks[D4];
  king_attack = kKingAttacks[D4];

  if (white_pawn_attack != 335544320ULL) {
    std::cout << "Failed when White Pawn is on D4.\n";
    return false;
  }
  if (black_pawn_attack != 21990232555520ULL) {
    std::cout << "Failed when Black Pawn is on D4.\n";
    return false;
  }
  if (knight_attack != 5666883501293568ULL) {
    std::cout << "Failed when Knight is on D4.\n";
    return false;
  }
  if (king_attack != 30872694685696ULL) {
    std::cout << "Failed when King is on D4.\n";
    return false;
  }

  white_pawn_attack = kPawnAttacks[WHITE][A5];
  black_pawn_attack = kPawnAttacks[BLACK][A5];
  knight_attack = kKnightAttacks[A5];
  king_attack = kKingAttacks[A5];

  if (white_pawn_attack != 131072ULL) {
    std::cout << "Failed when White Pawn is on A5.\n";
    return false;
  }
  if (black_pawn_attack != 8589934592ULL) {
    std::cout << "Failed when Black Pawn is on A5.\n";
    return false;
  }
  if (knight_attack != 2216203387392ULL) {
    std::cout << "Failed when Knight is on A5.\n";
    return false;
  }
  if (king_attack != 12918652928ULL) {
    std::cout << "Failed when King is on A5.\n";
    return false;
  }

  white_pawn_attack = kPawnAttacks[WHITE][H8];
  black_pawn_attack = kPawnAttacks[BLACK][H8];
  knight_attack = kKnightAttacks[H8];
  king_attack = kKingAttacks[H8];

  if (white_pawn_attack != 0ULL) {
    std::cout << "Failed when White Pawn is on H8.\n";
    return false;
  }
  if (black_pawn_attack != 16384ULL) {
    std::cout << "Failed when Black Pawn is on H8.\n";
    return false;
  }
  if (knight_attack != 4202496ULL) {
    std::cout << "Failed when Knight is on H8.\n";
    return false;
  }
  if (king_attack != 49216ULL) {
    std::cout << "Failed when King is on H8.\n";
    return false;
  }

  return true;
}
