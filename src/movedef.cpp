#include <bitboards.hpp>
#include <iostream>
#include <movedef.hpp>
#include <string>
#include <types.hpp>

std::string move_to_string(Move move) {
  int source_square = get_move_source_sq(move);
  int target_square = get_move_target_sq(move);

  std::string move_string;
  move_string += static_cast<char>('a' + (source_square % 8));
  move_string += static_cast<char>('1' + (7 - (source_square / 8)));
  move_string += static_cast<char>('a' + (target_square % 8));
  move_string += static_cast<char>('1' + (7 - (target_square / 8)));

  MoveFlag flag = get_move_flag(move);
  switch (flag) {
    case QUEEN_PROM:
      move_string += "q";
      break;
    case ROOK_PROM:
      move_string += "r";
      break;
    case BISHOP_PROM:
      move_string += "b";
      break;
    case KNIGHT_PROM:
      move_string += "n";
      break;
    case CAP_QUEEN_PROM:
      move_string += "q";
      break;
    case CAP_ROOK_PROM:
      move_string += "r";
      break;
    case CAP_BISHOP_PROM:
      move_string += "b";
      break;
    case CAP_KNIGHT_PROM:
      move_string += "n";
      break;
    default:
      break;
  }
  return move_string;
}

void print_move(Move mv) {
  Square source_sq = get_move_source_sq(mv);
  Square target_sq = get_move_target_sq(mv);
  MoveFlag flag = get_move_flag(mv);

  int source_rank = 8 - (source_sq / 8);
  int target_rank = 8 - (target_sq / 8);

  std::string source_sq_text, target_sq_text, flag_text;

  source_sq_text += static_cast<char>((source_sq % 8) + 'a');
  target_sq_text += static_cast<char>((target_sq % 8) + 'a');

  source_sq_text += std::to_string(source_rank);
  target_sq_text += std::to_string(target_rank);

  switch (flag) {
    case (QUIET):
      flag_text = "Quiet";
      break;
    case (DOUBLE_PUSH):
      flag_text = "Double push";
      break;
    case (KING_CASTLE):
      flag_text = "Kingside castle";
      break;
    case (QUEEN_CASTLE):
      flag_text = "Queenside castle";
      break;
    case (CAPTURE):
      flag_text = "Standard Capture";
      break;
    case (ENPASS_CAP):
      flag_text = "Enpassent Capture";
      break;
    case (QUEEN_PROM):
      flag_text = "Queen promotion";
      break;
    case (ROOK_PROM):
      flag_text = "Rook promotion";
      break;
    case (BISHOP_PROM):
      flag_text = "Bishop promotion";
      break;
    case (KNIGHT_PROM):
      flag_text = "Knight promotion";
      break;
    case (CAP_QUEEN_PROM):
      flag_text = "Capture and Queen promotion";
      break;
    case (CAP_ROOK_PROM):
      flag_text = "Capture and Rook promotion";
      break;
    case (CAP_BISHOP_PROM):
      flag_text = "Capture and Bishop promotion";
      break;
    case (CAP_KNIGHT_PROM):
      flag_text = "Capture and Knight promotion";
      break;
    default:
      break;
  }

  std::cout << "The Move is " << mv << ".\n";
  std::cout << "Source square is: " << source_sq_text << '\n';
  std::cout << "Target square is: " << target_sq_text << '\n';
  std::cout << "Move flag is: " << flag_text << "\n\n";
}