#pragma once

#include <vector>
#include "GameTypes.h"




const std::string BLUE = "\033[34m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string RESET = "\033[0m";

void renderGame(const Board& board, const Board& aiBoard);
void renderBoard(const Board& board);