#include <board.hpp>
#include <evaluate.hpp>
#include <movegen.hpp>
#include <search.hpp>

int negamax(Board& board, int depth, int alpha, int beta, int ply,
            SearchInfo& info) {
  if (depth == 0) return evaluate(board);
  MoveList movelist;
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
      if (score >= beta)
        return beta;
      else if (score > alpha)
        alpha = score;
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
  return alpha;
}

Move search_position(Board& board, SearchInfo& info, int depth) {
  Move best_move = 0;
  int best_score = -INFINITY_SCORE, score;
  MoveList movelist;

  generate_pseudo_legal_moves(board, movelist, ~0ULL);
  for (Move move : movelist) {
    board.make_move(move);
    if (board.is_position_legal()) {
      score =
          -negamax(board, depth - 1, -INFINITY_SCORE, INFINITY_SCORE, 0, info);
      if (score >= best_score) {
        best_score = score;
        best_move = move;
      }
    }
    board.undo_move(move);
  }

  if (best_move == 0) {
    std::cout << "Game Ended, either checkmate or stalemate!\n";
    return 0;
  }

  return best_move;
}
