#include <atomic>
#include <chrono>
#include <iostream>
#include <movegen.hpp>
#include <sstream>
#include <string>
#include <thread>
#include <uci.hpp>

long long get_current_time_ms() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

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
      info.time_set = false;

      int time = -1, inc = 0;
      int depth = 64;

      std::string arg;
      while (iss >> arg) {
        if (arg == "wtime" && board.get_side_to_move() == WHITE) {
          iss >> time;
        } else if (arg == "btime" && board.get_side_to_move() == BLACK) {
          iss >> time;
        } else if (arg == "winc" && board.get_side_to_move() == WHITE) {
          iss >> inc;
        } else if (arg == "binc" && board.get_side_to_move() == BLACK) {
          iss >> inc;
        } else if (arg == "movetime") {
          iss >> time;
        } else if (arg == "depth") {
          iss >> depth;
        }
      }

      if (time != -1) {
        info.time_set = true;
        info.time_limit = (time / 40) + (inc / 2);
        info.start_time = get_current_time_ms();

        if (info.time_limit > time - 50) {
          info.time_limit = time - 50;
        }
      }

      if (search_thread.joinable()) {
        search_thread.join();
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