#pragma once

#include <array>
#include <cstdint>
#include <types.hpp>

// Pseudo Random Number Generator
class PRNG {
 private:
  uint64_t state;

 public:
  constexpr explicit PRNG(uint64_t seed) : state(seed) {}

  constexpr uint64_t next() {
    state ^= state >> 12;
    state ^= state << 25;
    state ^= state >> 27;
    return state * 0x2545F4914F6CDD1DULL;
  }
};

struct ZobristKeys {
  // hash for all the pieces when placed in a each square
  std::array<std::array<uint64_t, 64>, NB_PIECES> pieces;
  // hash for all 16 states of castling rights
  std::array<uint64_t, 16> castling;
  // hash for all files enpassent is possible
  std::array<uint64_t, 8> enpassentFile;
  // hash for side to move
  uint64_t sideToMove;
};

constexpr ZobristKeys initialize_zobrist();

#include <details/zobrist_inline.inl>

inline constexpr ZobristKeys zkeys = initialize_zobrist();