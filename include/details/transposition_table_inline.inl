#include <board.hpp>
#include <transposition_table.hpp>

inline void TranspositionTable::write(uint64_t hashValue, int evaluation,
                                      TT_FLAG flag, int depth, Move move) {
  // faster than modulo
  uint64_t index = hashValue & (num_entries - 1);

  if ((table[index].depth <= depth) || (table[index].key != hashValue) ||
      (flag == TT_EXACT && (table[index].flag != TT_EXACT))) {
    table[index].key = hashValue;
    table[index].score = evaluation;
    table[index].depth = depth;
    table[index].move = move;
    table[index].flag = flag;
  }
}

inline TranspositionTableEntry TranspositionTable::read(uint64_t hashValue) {
  uint64_t index = hashValue & (num_entries - 1);
  if (table[index].key == hashValue) {
    return table[index];
  }

  TranspositionTableEntry invalidEntry;
  invalidEntry.flag = TT_NULL;
  return invalidEntry;
}