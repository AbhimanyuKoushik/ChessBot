#pragma once
#include <board.hpp>
#include <search.hpp>

void uci_loop(Board& board, SearchInfo& info);
Move parse_move(std::string move_string, Board& board);