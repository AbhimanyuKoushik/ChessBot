#include <board.hpp>
#include <transposition_table.hpp>

TranspositionTable::TranspositionTable(uint64_t size_in_mb) {
  uint64_t size_in_bytes = size_in_mb * 1024 * 1024;
  uint64_t entries_requested = size_in_bytes / sizeof(TranspositionTableEntry);

  // num_entries to be a power of 2
  num_entries = 1;
  while (num_entries <= entries_requested) {
    num_entries <<= 1;
  }
  num_entries >>= 1;

  table = new TranspositionTableEntry[num_entries];
  clear();
}

TranspositionTable::~TranspositionTable() { delete[] table; }

void TranspositionTable::clear() {
  for (uint64_t i = 0; i < num_entries; i++) {
    table[i].key = 0;
    table[i].score = 0;
    table[i].move = 0;
    table[i].flag = TT_NULL;
    table[i].depth = 0;
  }
}
