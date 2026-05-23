#include "board.hpp"
#include "search.hpp"
#include "uci.hpp"

int main() {
  // Instantiate the core state objects
  Board board;
  SearchInfo info;
  info.stopped = false;
  info.nodes = 0;

  // Load the standard starting position
  board.InitializeBoard();

  // Pass control to the blocking UCI I/O loop
  uci_loop(board, info);

  // The program will only reach this line when the GUI sends the "quit" command
  return 0;
};