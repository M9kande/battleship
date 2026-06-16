#include <vector>
#pragma once

typedef std::vector<std::vector<int>> Board;

void renderGame(const Board& board, const Board& aiBoard);
void renderBoard(const Board& board);