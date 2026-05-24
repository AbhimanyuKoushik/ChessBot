#include <board.hpp>
#include <evaluate.hpp>
#include <movegen.hpp>
#include <search.hpp>
#include <uci.hpp>

int quiescene(Board& board, int alpha, int beta, int ply, SearchInfo& info) {
  // base case and hatch
  if ((info.nodes & 2047) == 0) {
    if (info.time_set) {
      if (get_current_time_ms() - info.start_time > info.time_limit) {
        info.stopped = true;
      }
    }
    if (info.stopped) return 0;
  }

  info.nodes++;

  int standard_eval_result = evaluate(board);

  if (standard_eval_result >= beta) {
    return beta;
  }

  if (alpha < standard_eval_result) {
    alpha = standard_eval_result;
  }

  MoveList movelist;
  Color opp_side = (board.get_side_to_move() == WHITE ? BLACK : WHITE);
  Bitboard target_mask = board.get_side_occupancy(opp_side);
  Square enpass_sq = board.get_enpassent_sq();

  if (enpass_sq != NB_SQ) {
    target_mask |= (1 << enpass_sq);
  }

  uint64_t last_rank, pawn_pushes;
  if (opp_side == WHITE) {
    pawn_pushes = board.get_piece_bitboard(W_PAWN) << 8;
    last_rank = pawn_pushes & 0xFF'00'00'00'00'00'00'00;
  } else {
    pawn_pushes = board.get_piece_bitboard(B_PAWN) << 8;
    last_rank = pawn_pushes & 0x00'00'00'00'00'00'00'FF;
  }

  target_mask |= last_rank;

  // Only generates promotion and capture moves
  generate_pseudo_legal_moves(board, movelist, target_mask);

  for (Move move : movelist) {
    board.make_move(move);
    if (!board.is_position_legal()) {
      board.undo_move(move);
      continue;
    }

    int score = -quiescene(board, -beta, -alpha, ply + 1, info);

    board.undo_move(move);

    if (score >= beta) {
      return beta;
    } else if (score > alpha) {
      alpha = score;
    }
  }

  return alpha;
}

int negamax(Board& board, int depth, int alpha, int beta, int ply,
            SearchInfo& info) {
  // base case and hatch
  if ((info.nodes & 2047) == 0) {
    if (info.time_set) {
      if (get_current_time_ms() - info.start_time > info.time_limit) {
        info.stopped = true;
      }
    }
    if (info.stopped) return 0;
  }
  if (depth == 0) return quiescene(board, alpha, beta, ply, info);

  // querying the table
  TranspositionTableEntry entry = info.tt->read(board.get_current_hash());
  if (entry.flag != TT_NULL) {
    if (entry.depth >= depth) {
      int score = entry.score;
      if (score > MATE_SCORE - 100)
        score -= ply;
      else if (score < -MATE_SCORE + 100)
        score += ply;

      if (entry.flag == TT_EXACT) {
        return score;  // Return adjusted score
      } else if (entry.flag == TT_BETA) {
        if (score >= beta) return beta;
      } else if (entry.flag == TT_ALPHA) {
        if (score <= alpha) return alpha;
      }
    }
  }

  int original_alpha = alpha;
  MoveList movelist;
  Move best_move = 0;

  generate_pseudo_legal_moves(board, movelist, ~0ULL);
  int number_of_legalmoves = 0;

  for (Move move : movelist) {
    board.make_move(move);
    info.nodes++;
    if (!board.is_position_legal()) {
      board.undo_move(move);
    } else {
      number_of_legalmoves++;
      int score = -negamax(board, depth - 1, -beta, -alpha, ply + 1, info);
      board.undo_move(move);

      if (score >= beta) {
        int write_score = beta;
        if (write_score > MATE_SCORE - 100)
          write_score += ply;
        else if (write_score < -MATE_SCORE + 100)
          write_score -= ply;

        info.tt->write(board.get_current_hash(), write_score, TT_BETA, depth,
                       move);
        return beta;
      } else if (score > alpha) {
        alpha = score;
        best_move = move;
      }
    }
  }

  if (number_of_legalmoves == 0) {
    Color our_side = board.get_side_to_move();
    Color opp_side = (our_side == WHITE ? BLACK : WHITE);
    Piece our_king = (our_side == WHITE ? W_KING : B_KING);
    Square our_king_square =
        static_cast<Square>(get_lsb_index(board.get_piece_bitboard(our_king)));

    bool is_king_attacked = board.is_square_attacked(our_king_square, opp_side);
    if (is_king_attacked)
      return -MATE_SCORE + ply;
    else
      return 0;
  }

  int write_score = alpha;
  if (write_score > MATE_SCORE - 100)
    write_score += ply;
  else if (write_score < -MATE_SCORE + 100)
    write_score -= ply;

  if (alpha > original_alpha) {
    info.tt->write(board.get_current_hash(), write_score, TT_EXACT, depth,
                   best_move);
  } else {
    // 0 is safely passed because best_move was initialized to 0
    info.tt->write(board.get_current_hash(), write_score, TT_ALPHA, depth, 0);
  }

  return alpha;
}

Move search_position(Board& board, SearchInfo& info, int maxDepth) {
  Move global_best_move = 0;

  // Iterative deepening loop
  for (int current_depth = 1; current_depth <= maxDepth; current_depth++) {
    Move current_best_move = 0;
    int best_score = -INFINITY_SCORE, score;
    MoveList movelist;

    generate_pseudo_legal_moves(board, movelist, ~0ULL);
    for (Move move : movelist) {
      board.make_move(move);
      if (board.is_position_legal()) {
        score = -negamax(board, current_depth - 1, -INFINITY_SCORE,
                         INFINITY_SCORE, 0, info);
        if (score >= best_score) {
          best_score = score;
          current_best_move = move;
        }
      }
      board.undo_move(move);
      if (info.stopped) break;
    }

    if (info.stopped) break;

    global_best_move = current_best_move;
  }

  if (global_best_move != 0) {
    std::cout << "bestmove " << move_to_string(global_best_move) << "\n";
  } else {
    std::cout << "bestmove (none)\n";
  }
  return global_best_move;
}
