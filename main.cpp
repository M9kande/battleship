#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <tuple>
#include <random>
#include <limits>

static int empty = -3;
static int miss = -2;
static int hit = -1;

static int boardSize = 10;
static const int timeout = 1500;

static std::random_device rd;
static std::mt19937 g(rd());

// Initialize globally with -3 immediately
std::vector<std::vector<int>> board(boardSize, std::vector<int>(boardSize, empty));

std::vector<std::vector<int>> aiBoard(boardSize, std::vector<int>(boardSize, empty));

void renderGame(const std::vector<std::vector<int>>& board, const std::vector<std::vector<int>>& aiBoard);
void renderBoard(const std::vector<std::vector<int>>& board);

void system_clear() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

typedef std::vector<std::pair<int, int>> Vec4;

typedef std::vector<int> Vec2;

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

AiTurn ai;

std::array<int, 4> aiSunkShipsArray = {0,0,0,0};

struct Ship {
    int id = 0;
    int size = 0;
    std::vector<std::pair<int, int>> coordinates; // Store coordinates as pairs of (x, y)
    bool isSunk = false;
    

    Ship(int id, int size){
        this->id = id;
        this->size = size;
    }
    
    // General function to handle placing ships
    void placeShip(std::vector<std::vector<int>> &board, int numCoords) {
        bool isValid = false;
        static std::string Position[] = {"starting", "ending"};
        while (!isValid) {
            coordinates.clear();

            // Input start and end coordinates
            while(coordinates.size() < 2) {
                char x_in;
                int y_in;
                
                std::cout << "Input " << Position[coordinates.size()] << " coordinate for ship #" << id << " using a letter and a number: ";
                if (!(std::cin >> x_in >> y_in)) { 
                    if (std::cin.eof()) {
                        std::cout << "\nInput stream closed. Exiting.\n";
                        exit(0); // Stop the program safely
                    }

                    std::cin.clear(); 
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    std::cout << "Error: Enter a Letter and a Number.\n";
                    continue; 
                }

                x_in = toupper(x_in) - 'A';  // Convert letter to integer index
                y_in = y_in - 1;    // Adjust for 0-based indexing

                if (x_in < 0 || x_in >= boardSize || y_in < 0 || y_in >= boardSize) {
                    std::cout << "Coordinates out of bounds. Please try again.\n";
                    x_in = -1;
                    y_in = -1;
                    continue;
                }

                coordinates.emplace_back(x_in, y_in);
            }

            // Sort coordinates to handle reverse input gracefully
            sort(coordinates.begin(), coordinates.end());

            int startX = coordinates[0].first;
            int startY = coordinates[0].second;
            int endX = coordinates[1].first;
            int endY = coordinates[1].second;

            bool isHorizontal = (startY == endY);
            bool isVertical = (startX == endX);

            if (!(isHorizontal || isVertical)) {
                std::cout << "Ship coordinates must be in a straight line (horizontal or vertical). Try again.\n";
                continue;
            }

            if(isHorizontal){
                if((endX - startX - 1) > (numCoords - 2)){ // -2 because every ship has a minimum of 2 parts
                    std::cout << "Ship #" << id << "coordinates are too far apart\n";
                    continue;
                } 
                else if((endX - startX - 1) < (numCoords - 2)){
                    std::cout << "Ship #" << id << " coordinates are too close to each other\n";
                    continue;
                }
            } 
            else if(isVertical){
                if((endY - startY - 1) > (numCoords - 2)){
                    std::cout << "Ship #" << id << " coordinates are too far apart\n";
                    continue;
                } 
                else if((endY - startY - 1) < (numCoords - 2)){
                    std::cout << "Ship #" << id << " coordinates are too close to each other\n";
                    continue;
                }
            }

            // Generate full ship coordinates
            coordinates.clear();
            if (isHorizontal) {
                for (int x = startX; x <= endX; ++x) {
                    coordinates.emplace_back(x, startY);
                }
            } 
            else if (isVertical) {
                for (int y = startY; y <= endY; ++y) {
                    coordinates.emplace_back(startX, y);
                }
            }

            // Check for overlap
            bool overlap = false;
            for (const auto& coord : coordinates) {
                int x = coord.first;
                int y = coord.second;
                if (board[y][x] != empty) {
                    std::cout << "Space already occupied. Try again.\n";
                    overlap = true;
                    break;
                }
            }

            if (!overlap) {
                for (const auto& coord : coordinates) {
                    int x = coord.first;
                    int y = coord.second;
                    board[y][x] = id;  // Place the ship on the board
                }
                isValid = true;
                std::cout << "Ship #" << id << " placed successfully.\n";
                system_clear();
                renderBoard(board);
                std::cout << "\n";
            }
        }
    }


    void placeAiShip(std::vector<std::vector<int>> &aiBoard, int aiNumCoords){
        bool isValid = false;
        while(!isValid){
            std::vector<std::pair<int, int>> coordinates;
            int direction = rand() % 2; // 0 = horizontal, 1 = vertical
            int xStart = rand() % boardSize;
            int yStart = rand() % boardSize;

            for(int i = 0; i < aiNumCoords; i++){
                int x = xStart + (direction == 0 ? i : 0);
                int y = yStart + (direction == 1 ? i : 0);

                if(x >= boardSize || y >= boardSize){
                    break; //out of bounds
                }
                
                coordinates.emplace_back(x,y);

            }
            /*
                0 0 0 0 0 0 0 0
                0 0 0 0 0 0 0 0
                0 0 0 0 0 0 0 0
                0 0 0 0 0 0 0 0
                0 0 0 0 0 0 0 0
                0 0 0 0 0 0 0 0
                0 0 0 0 0 0 0 0
                0 0 0 0 0 0 0 0 
            */
            //check overlap
            if(coordinates.size() == aiNumCoords){
                bool overlap = false;
                for(auto &coord: coordinates){
                    if(aiBoard[coord.second][coord.first] != empty){
                        overlap = true;
                        break;
                    }
                }
                if(!overlap){
                    for(auto &coord: coordinates){
                        aiBoard[coord.second][coord.first] = id;

                    }
                    isValid = true;
                }
            }
        }
    }

    bool hitCheck() {
        size--;
        if (size <= 0) {
            isSunk = true;
            return true;
        }
        return false;
    }
};



void renderBoard(const std::vector<std::vector<int>>& board) {
    std::cout << "     ";
    for(int a = 0; a < boardSize; a++){
        std::cout << char(65+a) << " ";
    }
    std::cout << "\n";
    for (int i = 0; i < boardSize; i++) {
        std::cout << i + 1;
        if(i + 1 < 10){
            std::cout << "  |";
        }else{
            std::cout << " |";
        }
        for (int j = 0; j < boardSize; j++) {
            if (board[i][j] == empty) {
                std::cout << " ~";  // Water
            } else {
                std::cout << " " << board[i][j];
            }
        }
        std::cout << "\n";
    }
}

void renderGame(const std::vector<std::vector<int>>& board, const std::vector<std::vector<int>>& aiBoard) {
    std::cout << "-----PLAYER BOARD-----         -----AI BOARD-----     -----Sunk Ships-----\n";
    std::cout << "     A B C D E F G H              A B C D E F G H\n";
    for(int a = 0; a < boardSize; a++){
        std::cout << char(65+a) << " ";
    }

    for (int i = 0; i < boardSize; i++) {
        // Render player board
        std::cout << i + 1;
        if(i + 1 < 10){
            std::cout << "  |";
        }else{
            std::cout << " |";
        }
        for (int j = 0; j < boardSize; j++) {
            if (board[i][j] == empty) {
                std::cout << " ~";  // Water
            } else if (board[i][j] == miss) {
                std::cout << " O";  // Miss
            } else if (board[i][j] == hit) {
                std::cout << " X";  // Hit
            } else{
                std::cout << " " << board[i][j];
            }
        }

        // Spacer between boards
        std::cout << "          ";

        // Render computer board
        std::cout << i + 1 << " |";
        for (int j = 0; j < boardSize; j++) {
            if (aiBoard[i][j] == miss) {
                std::cout << " O";  // Miss
            } else if (aiBoard[i][j] == hit) {
                std::cout << " X";  // Hit
            } else{
                std::cout << " ~"; // Water
            }
        }

        std::cout << "          ";

        if(i <= 3){
            if(aiSunkShipsArray[i] != 0){
                for (int k = aiSunkShipsArray[i]; k > 0; k--){
                    std::cout << aiSunkShipsArray[i];
                }
            }
        }
        std::cout << "\n";
    }
}





//Players turns
auto playerTurn(std::vector<std::vector<int>> &aiBoard){
    system_clear();    
    renderGame(board, aiBoard);
    int shipHit = 0; //Default value;
    char x_in;
    int y_in;
    bool isValid = false;
    
    while(!isValid){
        std::cout << "-----PLAYER MOVE-----\n";
        std::cout << "Input coordinates of your hit: ";
        if (!(std::cin >> x_in >> y_in)) { 
            if (std::cin.eof()) {
                std::cout << "\nInput stream closed. Exiting.\n";
                exit(0); // Stop the program safely
            }
            
            std::cin.clear(); 
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "Error: Enter a Letter and a Number.\n";
            continue; 
        }
        int x = toupper(x_in) - 'A';  // Convert letter to integer index
        int y = toupper(y_in) - 1;    // Adjust for 0-based indexing

        // Ensure coordinates are within bounds
        if (x < 0 || x >= boardSize || y < 0 || y >= boardSize) {
            std::cout << "Coordinates out of bounds. Please try again.\n";
        continue;
        }
        if(aiBoard[y][x] == hit || aiBoard[y][x] == miss) {
            std::cout << "You already used this spot, try again\n";
            continue;
        } else if(aiBoard[y][x] != empty){
            isValid = true;
            shipHit = aiBoard[y][x];
            aiBoard[y][x] = hit; // hit
            system_clear();        
            renderGame(board, aiBoard);
            std::cout << "-----PLAYER MOVE-----\n";
            std::cout << x_in << y_in << '\n';
            std::cout << "HIT\n";

        } else{
            isValid = true;
            
            aiBoard[y][x] = miss; // miss
            system_clear();        
            renderGame(board, aiBoard);
            std::cout << "-----PLAYER MOVE-----\n";
            std::cout << x_in << y_in << '\n';
            std::cout << "MISS\n";
        }

    }
    return shipHit;
}

void aiTurn(std::vector<std::vector<int>> &board, AiTurn &a){
    system_clear();
    renderGame(board, aiBoard);
    bool isValid = false;
    int x = 0;
    int y = 0;
    std::string out;
    while(!isValid){
        std::cout << "-----AI MOVE-----\n";
        if (!a.lastHit || a.lastSunk) {
            if (a.huntMoves.empty()) a.initializeHuntMoves(); 

            std::pair<int, int> move = a.huntMoves.back();
            a.huntMoves.pop_back();
            
            x = move.first;
            y = move.second;
        } // ... inside your while(!isValid) loop
        else if(a.direction != '0') {
            if(a.direction == 'x'){
                if(!a.vec2.empty()){
                    x = a.lastX;
                    y = a.lastY;
                    int random = rand() % a.vec2.size();
                    x += a.vec2[random];
                    
                    // Using the swap-and-pop logic we discussed for safety
                    std::swap(a.vec2[random], a.vec2.back());
                    a.vec2.pop_back();
                }
                else {
                    // THE WALKER: Find the other side of the ship
                    int temp_x = a.lastX;
                    int step_x = (a.secondLastX - a.lastX); // The direction back toward the start

                    // Keep walking as long as we are on a 'hit'
                    while (temp_x >= 0 && temp_x < boardSize && board[a.lastY][temp_x] == hit) {
                        temp_x += step_x;
                    }
                    x = temp_x;
                    y = a.lastY;
                }
            }
            if(a.direction == 'y'){
                if(!a.vec2.empty()){
                    x = a.lastX;
                    y = a.lastY;
                    int random = rand() % a.vec2.size();
                    y += a.vec2[random];

                    std::swap(a.vec2[random], a.vec2.back());
                    a.vec2.pop_back();
                }
                else {
                    // THE WALKER: Find the other side of the ship
                    int temp_y = a.lastY;
                    int step_y = (a.secondLastY - a.lastY); // The direction back toward the start

                    // Keep walking as long as we are on a 'hit'
                    while (temp_y >= 0 && temp_y < boardSize && board[temp_y][a.lastX] == hit) {
                        temp_y += step_y;
                    }
                    y = temp_y;
                    x = a.lastX;
                }
            }
        }
        else{
            if(!a.vec4.empty()){
                    x = a.lastX + a.vec4.back().first;
                    y = a.lastY + a.vec4.back().second;
                    a.vec4.pop_back();
            } else{
                a.lastHit = false;
                continue;
            }
        }

        // Ensure coordinates are within bounds
        if (x < 0 || x >= boardSize || y < 0 || y >= boardSize) {
            continue;
        }
        if(board[y][x] == hit || board[y][x] == miss){
            continue;
        }
        else if(board[y][x] != empty){
            if(a.lastHit){
                if(x == a.lastX){
                    a.direction = 'y';
                    a.vec2 = {1, -1}; 
                     std::shuffle(a.vec2.begin(), a.vec2.end(), g);
                } else if(y == a.lastY){
                    a.direction = 'x';
                    a.vec2 = {1, -1}; 
                    std::shuffle(a.vec2.begin(), a.vec2.end(), g);
                }
            }

            a.shipHit = board[y][x];

            board[y][x] = hit; // hit
            a.secondLastX = a.lastX;
            a.secondLastY = a.lastY;
            a.lastX = x;
            a.lastY = y;
            a.lastHit = true;
            a.vec4 = {{0,1}, {0,-1}, {1,0}, {-1,0}};
            std::shuffle(a.vec4.begin(), a.vec4.end(), g);
            isValid = true;
            out = "AI HIT";
        }
        else{
            isValid = true;
            a.shipHit = 0;
            board[y][x] = miss; // miss
            out = "AI MISS";
        }
    }
    system_clear();
    renderGame(board, aiBoard);
    
    std::cout << "-----AI MOVE-----\n";
    
    std::cout << (char)(x + 'A') << y + 1 << '\n'; // print coords chose by computer
    std::cout << out <<"\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
}


int main(int argc, char* argv[]) {

    bool devMode = false;
    bool randomMode = false;

    bool isValid = false;
    std::cout << "Board size(max=20, default=10): ";
    while (!isValid) {
        if (!(std::cin >> boardSize)) { 
            if (std::cin.eof()) {
                std::cout << "\nInput stream closed. Exiting.\n";
                exit(0); 
            }

            std::cin.clear(); 
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "Error: Invalid input. Enter a number.\n";
            continue; 
        } 
        else {
            // Check if the number actually fits within your rules (e.g., between 2 and 20)
            if (boardSize > 20 || boardSize < 2) {
                std::cout << "Error: Enter a number between 2 and 20.\n";
                // Leave isValid as false so the loop runs again
            } else {
                isValid = true; // Input is completely safe!
            }
        }
    }
    


    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--dev") {
            devMode = true;
            break;
        }

        if (std::string(argv[i]) == "--random") {
            randomMode = true;
            break;
        }
    }

    int playerNumMoves = 0;
    int aiNumMoves = 0;
    srand(time(nullptr));

    int turn = 1; // First turn is the player's

    // Ship definitions for player and AI
    std::array<Ship, 4> playerShips = { Ship(2,2), Ship(3,3), Ship(4,4), Ship(5,5) };

    std::array<Ship, 4> aiShips = { Ship(2,2), Ship(3,3), Ship(4,4), Ship(5,5) };

    // Place AI ships
    for(auto &s : aiShips){
        s.placeAiShip(aiBoard, s.size);
    }

    // Place player ships

    system_clear();    
    if(devMode){
        board[0][0] = 2;
        board[1][0] = 2;

        board[1][2] = 3;
        board[1][3] = 3;
        board[1][4] = 3;

        board[2][6] = 4;
        board[3][6] = 4;
        board[4][6] = 4;
        board[5][6] = 4;

        board[7][1] = 5;
        board[7][2] = 5;
        board[7][3] = 5;
        board[7][4] = 5;
        board[7][5] = 5;
    } 
    else if(randomMode){
        for(auto &s : playerShips){
            s.placeAiShip(board, s.size);
        }
    }
    else{
        renderBoard(board);
        std::cout << "\n";
        for(int i = 0; i < playerShips.size();  i++){
            std::cout << "-----SHIP " <<playerShips[i].id << "-----\n";
            playerShips[i].placeShip(board, playerShips[i].size);
        }
        system_clear();
    }
        
    // Render the boards
    renderGame(board, aiBoard);
    
    ai.lastSunk = false;  
    ai.shipHit = -1;
    ai.lastX = -1;
    ai.lastY = -1;
    ai.secondLastX = -1;
    ai.secondLastY = -1;
    ai.lastHit = false;
    ai.direction = '0';
    int playerSunkCount = 0;
    int aiSunkCount = 0;
    
    // Game loop
    for(;;) {
        if (turn == 1) {  // PLAYER MOVE ######################
            int shipHit = playerTurn(aiBoard);
            playerNumMoves++;
            turn = 0;

            // Check if any AI ship is sunk
            if (shipHit > 0) {
                // Check AI Ships
                for(auto &s : aiShips) {
                    if (s.id == shipHit) {
                        if (s.hitCheck()) {
                            std::cout << "You sunk AI Ship " << s.id << "!\n";
                            aiSunkCount++;
                            aiSunkShipsArray[shipHit-2] = shipHit;
                        }
                    }
                }
            }

            // Check if player has lost
            
            std::this_thread::sleep_for(std::chrono::milliseconds(timeout));  // Wait a bit before next turn

        }
        else if (turn == 0) {  // AI MOVE ######################
            aiTurn(board, ai);
            aiNumMoves++;
            

            // After the AI turn, check if any AI ship is sunk
            if (ai.shipHit > 0) {
                // Check Player Ships
                for(auto &s : playerShips) {
                    if (s.id == ai.shipHit) {
                        if (s.hitCheck()) {
                            std::cout << "AI Sunk your Ship " << s.id << "!\n";
                            playerSunkCount++;
                            ai.reset(); // NOW we reset the AI memory to hunt mode
                        }
                    }
                }
            }

            turn = 1;
            std::this_thread::sleep_for(std::chrono::milliseconds(timeout));  // Wait a bit before next turn
        }
        if (playerSunkCount == 4) {
                std::cout << "AI has sunk all your ships. AI wins!\n";
                std::cout << "Number of moves: " << aiNumMoves << "\n";
                break;
            }
        if (aiSunkCount == 4) {
                std::cout << "Player has sunk all enemy ships. Player wins!\n";
                std::cout << "Number of moves: " << playerNumMoves << "\n";
                break;
            }
    }
    return 0;
}

