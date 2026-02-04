#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <tuple>

static int empty = -3;
static int miss = -2;
static int hit = -1;

// Initialize globally with -3 immediately
std::array<std::array<int, 8>, 8> board = []{
    std::array<std::array<int, 8>, 8> b;
    for(auto &row : b) row.fill(empty);
    return b;
}();

std::array<std::array<int, 8>, 8> aiBoard = []{
    std::array<std::array<int, 8>, 8> b;
    for(auto &row : b) row.fill(empty);
    return b;
}();

void renderGame(std::array<std::array<int, 8>, 8> board, std::array<std::array<int, 8>, 8> aiBoard);
void renderBoard(std::array<std::array<int, 8>, 8> board);

void system_clear() {
    #ifdef _WIN32
        system("cls");
        system("chcp 65001");
        system("color B0");
    #else
        system("clear");
    #endif
}

struct Ship {
    int id{};
    int size{};
    std::vector<std::pair<int, int>> coordinates; // Store coordinates as pairs of (x, y)
    bool isSunk = false;

    // General function to handle placing ships
    void placeShip(std::array<std::array<int, 8>, 8> &board, int numCoords) {
    bool isValid = false;

    while (!isValid) {
        coordinates.clear();

        // Input start and end coordinates
        for (int i = 0; i < 2; i++) {
            char x_in;
            int y_in;
            std::string Position[] = {"starting", "ending"};
            std::cout << "Input " << Position[i] << " coordinate for ship #" << id << " using a letter and a number: ";
            std::cin >> x_in >> y_in;

            int x = toupper(x_in) - 'A';  // Convert letter to integer index
            int y = toupper(y_in) - 1;    // Adjust for 0-based indexing

            if (x < 0 || x >= 8 || y < 0 || y >= 8) {
                std::cout << "Coordinates out of bounds. Please try again.\n";
                i--;  // Redo the input for this coordinate
                continue;
            }

            coordinates.emplace_back(x, y);
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
            if((endX - startX - 1) > (numCoords - 2)){
                std::cout << "Ship #" << id << "coordinates are too far apart\n";
                continue;
            } else if((endX - startX - 1) < (numCoords - 2)){
                std::cout << "Ship #" << id << " coordinates are too close to each other\n";
                continue;
            }
        } else if(isVertical){
            if((endY - startY - 1) > (numCoords - 2)){
                std::cout << "Ship #" << id << " coordinates are too far apart\n";
                continue;
            } else if((endY - startY - 1) < (numCoords - 2)){
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
        } else if (isVertical) {
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


    void placeAiShip(std::array<std::array<int, 8>, 8> &aiBoard, int aiNumCoords){
    bool isValid = false;
    while(!isValid){
        std::vector<std::pair<int, int>> coordinates;
        int direction = rand() % 2; // 0 = horizontal, 1 = vertical
        int xStart = rand() % 8;
        int yStart = rand() % 8;

        for(int i = 0; i < aiNumCoords; i++){
            int x = xStart + (direction == 0 ? i : 0);
            int y = yStart + (direction == 1 ? i : 0);

        if(x >= 8 || y >= 8){
            break; //out of bounds
        }
        coordinates.emplace_back(x,y);

        }
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
    bool playerSunkShips(){
      bool lastSunkShip = false;
      
      size--;
      if(size <= 0){
          std::cout << "Ship #" << id << " has been sunk\n";
          lastSunkShip = true;
          isSunk = true;
      }
      return lastSunkShip;
    }

    void aiSunkShips(){
        size--;
        if(size <= 0){
            std::cout << "Ai Ship #" << id << " has been sunk\n";
            isSunk = true;
        }
    }
};


void renderBoard(std::array<std::array<int, 8>, 8> board) {
    std::cout << "    A B C D E F G H\n";
    for (int i = 0; i < 8; i++) {
        std::cout << i + 1 << " |";
        for (int j = 0; j < 8; j++) {
            if (board[i][j] == empty) {
                std::cout << "🌊";  // Water
            } else {
                std::cout << " " << board[i][j];  // Display the ship's ID
            }
        }
        std::cout << "\n";
    }
}

void renderGame(std::array<std::array<int, 8>, 8> board, std::array<std::array<int, 8>, 8> aiBoard) {
    std::cout << "#####PLAYER BOARD######      #####AI BOARD#####\n";
    std::cout << "    A B C D E F G H              A B C D E F G H\n";

    for (int i = 0; i < 8; i++) {
        // Render player board
        std::cout << i + 1 << " |";
        for (int j = 0; j < 8; j++) {
            if (board[i][j] == empty) {
                std::cout << "🌊";  // Water
            } else if (board[i][j] == miss) {
                std::cout << "❎";  // Miss
            } else if (board[i][j] == hit) {
                std::cout << "💥";  // Hit
            } else {
                std::cout << " " << board[i][j];  // Display the ship's ID
            }
        }

        // Spacer between boards
        std::cout << "          ";

        // Render computer board
        std::cout << i + 1 << " |";
        for (int j = 0; j < 8; j++) {
            if (aiBoard[i][j] == miss) {
                std::cout << "❎";  // Miss
            } else if (aiBoard[i][j] == hit) {
                std::cout << "💥";  // Hit
            } else{
                std::cout << "🌊"; // Water
            }
        }
        std::cout << "\n";
    }
}


//Players turns
auto playerTurn(std::array<std::array<int, 8>, 8> &aiBoard){
    system_clear();    renderGame(board, aiBoard);
    int aiShipHit = 0; //Default value;
    char x_in;
    int y_in;
    bool isValid = false;
    while(!isValid){
    std::cout << "#####PLAYER MOVE#####\n";
    std::cout << "Input coordinates of your hit: ";
    std::cin >> x_in >> y_in;

    int x = toupper(x_in) - 'A';  // Convert letter to integer index
    int y = toupper(y_in) - 1;    // Adjust for 0-based indexing

    // Ensure coordinates are within bounds
    if (x < 0 || x >= 8 || y < 0 || y >= 8) {
    std::cout << "Coordinates out of bounds. Please try again.\n";
    continue;
    }
    if(aiBoard[y][x] == hit || aiBoard[y][x] == miss) {
        std::cout << "You already used this spot, try again\n";
        continue;
    } else if(aiBoard[y][x] != empty){
        isValid = true;
        switch(aiBoard[y][x]){
            case 2: aiShipHit = 2; break;
            case 3: aiShipHit = 3; break;
            case 4: aiShipHit = 4; break;
            case 5: aiShipHit = 5; break;
            default:
                std::cout << "Ai turn error\n";
        }
        aiBoard[y][x] = hit; // hit
        system_clear();        renderGame(board, aiBoard);
        std::cout << "#####PLAYER MOVE#####\n";
        std::cout << x_in << y_in << '\n';
        std::cout << "HIT\n";

    } else{
        isValid = true;
        
        switch(aiBoard[y][x]){
            case 2: aiShipHit = 2; break;
            case 3: aiShipHit = 3; break;
            case 4: aiShipHit = 4; break;
            case 5: aiShipHit = 5; break;
            default:
                std::cout << "Ai turn error\n";
        }
        aiBoard[y][x] = miss; // miss
        system_clear();        
        renderGame(board, aiBoard);
        std::cout << "#####PLAYER MOVE#####\n";
        std::cout << x_in << y_in << '\n';
        std::cout << "MISS\n";
    }

    }
    return aiShipHit;
}

std::tuple<int, bool, bool, int, int, char> aiTurn(std::array<std::array<int, 8>, 8> &board, bool lastHit, bool lastSunk, int last_x, int last_y, char direction){
    system_clear();
    renderGame(board, aiBoard);
    int shipHit;
    bool isValid = false;
    int x_out;
    int y_out;
    int x;
    int y;
    int attempts = 0;
    while(!isValid){
        std::cout << "#####AI MOVE#####\n";
        int x = last_x;
        int y = last_y;

        if (!lastHit || (last_x == hit && last_y == hit) || lastSunk) {
            x = rand() % 8;
            y = rand() % 8;
        } else if(direction != '0') {
            if(direction == 'x'){
                switch(rand() % 2){
                    case 0: x += 1; break;
                    case 1: x -= 1; break;
                }
            }
            if(direction == 'y'){
                switch(rand() % 2){
                    case 0: y += 1; break;
                    case 1: y -= 1; break;
                }
            }
        }
        else{
            int dir = rand() % 4;
            switch(dir){
                case 0: y += 1; break;
                case 1: x += 1; break;
                case 2: y -= 1; break;
                case 3: x -= 1; break;
            }
        }
        //Ten if w innym miejscu
        //Gdy mamy kierunek i próbę strzału >= 2 to idziemy pętlą w kierunku aż znajdziemy puste pole board[y][x] = empty
         if(attempts >= 2){
            lastHit = false;
            attempts = 0;
        }



        x_out = x + 'A';
        y_out = y + 1;
        // Ensure coordinates are within bounds
        if (x < 0 || x >= 8 || y < 0 || y >= 8) {
            continue;
        }
        if(board[y][x] == hit || board[y][x] == miss){
            attempts++;
            continue;
        }
        else if(board[y][x] != empty){
            if(lastHit){
                if(x - last_x == 0){
                    direction = 'y';
                } else if(y - last_y == 0){
                    direction = 'x';
                }
            }

            switch(board[y][x]){
                case 2: shipHit = 2; break;
                case 3: shipHit = 3; break;
                case 4: shipHit = 4; break;
                case 5: shipHit = 5; break;
            }

            board[y][x] = hit; // hit
            last_x = x;
            last_y = y;
            lastHit = true;
            isValid = true;

            system_clear();
            renderGame(board, aiBoard);

            std::cout << "#####AI MOVE#####\n";
           
            std::cout << (char)x_out << y_out << '\n'; // print coords chose by computer
            std::cout << "HIT\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        }
        else{
            isValid = true;
            board[y][x] = miss; // miss
            system_clear();
            renderGame(board, aiBoard);

            std::cout << "#####AI MOVE#####\n";
            
            std::cout << (char)x_out << y_out << '\n'; // print coords chose by computer
            std::cout << "MISS\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        }
    }
    return std::make_tuple(shipHit, lastHit, lastSunk, last_x, last_y, direction);
}


int main() {
    bool playerSunk = false;
    bool aiSunk = false;
    int playerNumMoves = 0;
    int aiNumMoves = 0;
    int Last_x = hit;
    int Last_y = hit;
    srand(time(nullptr));

    int turn = 1; // First turn is the player's

    // Ship definitions for player and AI
    Ship ship_2, ship_3, ship_4, ship_5;
    ship_2.id = 2; ship_3.id = 3; ship_4.id = 4; ship_5.id = 5;
    ship_2.size = 2; ship_3.size = 3; ship_4.size = 4; ship_5.size = 5;
    int playerShipsSize = ship_2.size + ship_3.size + ship_4.size + ship_5.size;

    Ship aiShip_2, aiShip_3, aiShip_4, aiShip_5;
    aiShip_2.id = 2; aiShip_3.id = 3; aiShip_4.id = 4; aiShip_5.id = 5;
    aiShip_2.size = 2; aiShip_3.size = 3; aiShip_4.size = 4; aiShip_5.size = 5;
    int aiShipsSize = aiShip_2.size + aiShip_3.size + aiShip_4.size + aiShip_5.size;

    // Place AI ships
    aiShip_2.placeAiShip(aiBoard, 2); // ship #2
    aiShip_3.placeAiShip(aiBoard, 3); // ship #3
    aiShip_4.placeAiShip(aiBoard, 4); // ship #4
    aiShip_5.placeAiShip(aiBoard, 5); // ship #5

    // Place player ships

    system_clear();    renderBoard(board);
    std::cout << "\n";
    std::cout << "#####SHIP 2#####\n";
    ship_2.placeShip(board, 2);  // Ship 2 has 2 coordinates
    std::cout << "#####SHIP 3#####\n";
    ship_3.placeShip(board, 3);  // Ship 3 has 3 coordinates
    std::cout << "#####SHIP 4#####\n";
    ship_4.placeShip(board, 4);  // Ship 4 has 4 coordinates
    std::cout << "#####SHIP 5#####\n";
    ship_5.placeShip(board, 5);  // Ship 5 has 5 coordinates
    system_clear();
    // Render the boards
    renderGame(board, aiBoard);

    bool lastSunkShip = false;  
    int shipHit, last_x, last_y = hit;
    bool lastHit = false;
    char direction = '0';
    // Game loop
    for(;;) {
        if (turn == 1) {  // PLAYER MOVE ######################
            int aiShipHit = playerTurn(aiBoard);
            playerNumMoves++;
            turn = 0;

            // Check if any AI ship is sunk
            switch(aiShipHit) {
                case 2:
                    aiShip_2.aiSunkShips();
                    aiShipsSize--;
                    break;
                case 3:
                    aiShip_3.aiSunkShips();
                    aiShipsSize--;
                    break;
                case 4:
                    aiShip_4.aiSunkShips();
                    aiShipsSize--;
                    break;
                case 5:
                    aiShip_5.aiSunkShips();
                    aiShipsSize--;
                    break;
                default:
                    break;
            }

            // Check if player has lost
            playerSunk = true;
            for (auto& ship : {ship_2, ship_3, ship_4, ship_5}) {
                if (!ship.isSunk) {
                    playerSunk = false;
                    break;
                }
            }

            if (playerSunk) {
                std::cout << "AI has sunk all your ships. AI wins!\n";
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));  // Wait a bit before next turn

        }
        else if (turn == 0) {  // AI MOVE ######################
            std::tuple<int, bool, bool, int, int, char> ai_result = aiTurn(board, lastHit, lastSunkShip, last_x, last_y, direction);
            shipHit = std::get<0>(ai_result);
            lastHit = std::get<1>(ai_result);
            last_x = std::get<3>(ai_result);
            last_y = std::get<4>(ai_result);
            direction = std::get<5>(ai_result);
            aiNumMoves++;
            turn = 1;

            // After the AI turn, check if any AI ship is sunk
            switch(shipHit) {
                case 2:
                    lastSunkShip = ship_2.playerSunkShips(); // Check if ship #2 is sunk
                    playerShipsSize--;
                    break;
                case 3:
                    lastSunkShip = ship_3.playerSunkShips(); // Check if ship #3 is sunk
                    playerShipsSize--;
                    break;
                case 4:
                    lastSunkShip = ship_4.playerSunkShips(); // Check if ship #4 is sunk
                    playerShipsSize--;
                    break;
                case 5:
                    lastSunkShip = ship_5.playerSunkShips(); // Check if ship #5 is sunk
                    playerShipsSize--;
                    break;
                default:
                    continue;
            }

            // Check if all AI ships are sunk
            aiSunk = true;
            for (auto& aiShip : {aiShip_2, aiShip_3, aiShip_4, aiShip_5}) {
                if (!aiShip.isSunk) {
                    aiSunk = false;
                    break;
                }
            }

            if (aiSunk) {
                std::cout << "Player has sunk all enemy ships. Player wins!\n";
                std::cout << "Number of moves: " << playerNumMoves;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));  // Wait a bit before next turn
        }
    }
    system("color 00");
    return 0;
}

