#include <vector>
typedef std::vector<std::vector<int>> Board;
typedef std::vector<std::pair<int, int>> Vec4;

typedef std::vector<int> Vec2;

bool devMode = false;
bool randomMode = false;

static int empty = -3;
static int miss = -2;
static int hit = -1;

static int boardSize = 26;
