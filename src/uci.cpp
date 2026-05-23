#include <iostream>
#include <movegen.hpp>
#include <sstream>
#include <string>
#include <thread>
#include <uci.hpp>

void uci_loop(Board& board, SearchInfo& info) {
  std::string line;
  std::thread search_thread;

  while (std::getline(std::cin, line)) {
    std::istringstream iss(line);
    std::string token;

    iss >> token;

    if (token == "uci") {
      std::cout << "id name Lumin\n";
      std::cout << "id author Abhimanyu\n";
      std::cout << "uciok\n";
    }

    else if (token == "isready") {
      std::cout << "readyok\n";
    }

    else if (token == "go") {
      info.stopped = false;
      info.nodes = 0;

      if (search_thread.joinable()) {
        search_thread.join();
      }

      int depth = 5;
      std::string arg;
      while (iss >> arg) {
        if (arg == "depth" && iss >> arg) {
          depth = std::stoi(arg);
        }
      }

      search_thread =
          std::thread(search_position, std::ref(board), std::ref(info), depth);
    }

    else if (token == "stop") {
      info.stopped = true;
      if (search_thread.joinable()) {
        search_thread.join();
      }
    }

    else if (token == "quit") {
      info.stopped = true;
      if (search_thread.joinable()) search_thread.join();
      break;
    }

    else if (token == "position") {
      std::string position_details;
      iss >> position_details;
      if (position_details == "startpos") {
        board.InitializeBoard();
      } else if (position_details == "fen") {
        std::string fen_string;
        std::string fen_token;
        for (size_t i = 0; i < 6; i++) {
          iss >> fen_token;
          fen_string += fen_token;
          if (i < 5) fen_string += " ";
        }
        board.LoadFEN(fen_string);
      }
      std::string next_token;
      while (iss >> next_token && next_token == "moves") {
        std::string move_string;
        while (iss >> move_string) {
          Move move = parse_move(move_string, board);
          if (move != 0) board.make_move(move);
        }
      }
    }
  }
}

Move parse_move(std::string move_string, Board& board) {
  MoveList movelist;
  generate_pseudo_legal_moves(board, movelist, ~0ULL);
  std::string current_move_string;

  for (Move move : movelist) {
    current_move_string = move_to_string(move);
    if (current_move_string == move_string) {
      board.make_move(move);
      if (board.is_position_legal()) {
        board.undo_move(move);
        return move;
      } else
        board.undo_move(move);
    }
  }

  return 0;
}