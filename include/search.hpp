#pragma once

#include <atomic>
#include <board.hpp>
#include <movegen.hpp>
#include <transposition_table.hpp>

// Limits to prevent overflow
constexpr int INFINITY_SCORE = 32000;
constexpr int MATE_SCORE = 30000;
constexpr int MATE_BOUND = 29000;  // Any score greater than 29000 is mate

// Tracks data for the search tree
struct SearchInfo {
  uint64_t nodes;
  std::atomic<bool> stopped;

  // Used for time management
  bool time_set;
  long long start_time;
  long long time_limit;

  // transposition table pointer
  TranspositionTable* tt;
};

Move search_position(Board& board, SearchInfo& info, int maxDepth);

int negamax(Board& board, int depth, int alpha, int beta, int ply,
            SearchInfo& info);

inline void score_moves(const Board& board, MoveList& movelist, Move tt_move);

#include <details/search_inline.inl>