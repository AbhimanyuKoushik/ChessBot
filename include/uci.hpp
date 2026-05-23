#pragma once
#include <board.hpp>
#include <search.hpp>

long long get_current_time_ms();
void uci_loop(Board& board, SearchInfo& info);
Move parse_move(std::string move_string, Board& board);