#pragma once

#include <board.hpp>
#include <cstdint>

enum TT_FLAG : uint8_t {
  TT_EXACT,  // Position was evaluated exact calculation
  TT_ALPHA,  // Position was worse than best case, hence not exactly evaluated
  TT_BETA,   // Position was too good than opponent wouldn't play, hence not
             // exactly evaluated
  TT_NULL    // For showing that entry is invalid
};

struct alignas(16) TranspositionTableEntry {
  uint64_t key;
  int16_t score;
  Move move;
  TT_FLAG flag;
  uint8_t depth;

  // So far 14 bytes, we will pad with 2 bytes so that it will be easier for
  // reading data
};

class TranspositionTable {
 private:
  TranspositionTableEntry* table;
  uint64_t num_entries;

 public:
  TranspositionTable(uint64_t size_in_mb);

  ~TranspositionTable();

  void clear();

  inline void write(uint64_t hashValue, int evaluation, TT_FLAG flag, int depth,
                    Move move);

  inline TranspositionTableEntry read(uint64_t hashValue);
};

#include <details/transposition_table_inline.inl>
