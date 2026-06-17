#include "computer.h"
struct AiTurn{
    int shipHit = 0;
    bool lastHit = false;
    bool lastSunk = false;
    

    int lastX = -1;
    int lastY = -1;
    int secondLastX = -1;
    int secondLastY = -1;
    
    char direction = '0';
    Vec2 vec2; 
    Vec4 vec4;

    std::vector<std::pair<int, int>> huntMoves;

    void reset(){
        direction = '0';
        vec2.clear();
        vec4.clear();
        shipHit = 0;
        lastHit = false;
        lastSunk = false;
        lastX = -1; lastY = -1;
        secondLastX = -1; secondLastY = -1;
    }

    void initializeHuntMoves() {
    huntMoves.clear();
    for (int y = 0; y < boardSize; y++) {
        for (int x = 0; x < boardSize; x++) {
            // Parentheses are vital here!
            if ((x + y) % 2 == 0) {
                huntMoves.push_back({x, y});
            }
        }
    }
    // Shuffle once using the Mersenne Twister
    // Note: ensure 'g' (your random engine) is defined globally or passed in
    std::shuffle(huntMoves.begin(), huntMoves.end(), g);
}
};