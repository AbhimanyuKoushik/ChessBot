#pragma once

#include <types.hpp>
#include <zobrist.hpp>

constexpr ZobristKeys initialize_zobrist() {
  ZobristKeys keys = {};

  PRNG rng(0xABCDEFABCDEF6789ULL);

  for (int piece = 0; piece < NB_PIECES; piece++) {
    for (int square = 0; square < 64; square++) {
      keys.pieces[piece][square] = rng.next();
    }
  }

  for (int i = 0; i < 16; i++) {
    keys.castling[i] = rng.next();
  }

  for (int i = 0; i < 8; i++) {
    keys.enpassentFile[i] = rng.next();
  }

  keys.sideToMove = rng.next();

  return keys;
}