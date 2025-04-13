#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <tuple>

using namespace std;

vector<vector<int>> boardMake();
vector<vector<int>> computerBoardMake();
void renderGame(vector<vector<int>> board, vector<vector<int>> aiBoard);
void renderBoard(vector<vector<int>> board);

vector<vector<int>> board = boardMake();
vector<vector<int>> aiBoard = computerBoardMake();

void system_clear() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

struct Ship {
    int id{};
    int size{};
    vector<pair<int, int>> coordinates; // Store coordinates as pairs of (x, y)
    bool isSunk = false;

    // General function to handle placing ships
    void placeShip(vector<vector<int>> &board, int numCoords) {
    bool isValid = false;

    while (!isValid) {
        coordinates.clear();

        // Input start and end coordinates
        for (int i = 0; i < 2; i++) {
            char x_in;
            int y_in;
            string Position[] = {"starting", "ending"};
            cout << "Input " << Position[i] << " coordinate for ship #" << id << " using an upper case letter and a number: ";
            cin >> x_in >> y_in;

            int x = x_in - 'A';  // Convert letter to integer index
            int y = y_in - 1;    // Adjust for 0-based indexing

            if (x < 0 || x >= 8 || y < 0 || y >= 8) {
                cout << "Coordinates out of bounds. Please try again.\n";
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
            cout << "Ship coordinates must be in a straight line (horizontal or vertical). Try again.\n";
            continue;
        }

        if(isHorizontal){
            if((endX - startX - 1) > (numCoords - 2)){
                cout << "Ship #" << id << "coordinates are too far apart\n";
                continue;
            } else if((endX - startX - 1) < (numCoords - 2)){
                cout << "Ship #" << id << " coordinates are too close to each other\n";
                continue;
            }
        } else if(isVertical){
            if((endY - startY - 1) > (numCoords - 2)){
                cout << "Ship #" << id << " coordinates are too far apart\n";
                continue;
            } else if((endY - startY - 1) < (numCoords - 2)){
                cout << "Ship #" << id << " coordinates are too close to each other\n";
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
            if (board[y][x] != -3) {
                cout << "Space already occupied. Try again.\n";
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
            cout << "Ship #" << id << " placed successfully.\n";
            system_clear();
            renderBoard(board);
            cout << "\n";
        }
    }
}


    void placeAiShip(vector<vector<int>> &aiBoard, int aiNumCoords){
    bool isValid = false;
    while(!isValid){
        vector<pair<int, int>> coordinates;
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
                if(aiBoard[coord.second][coord.first] != -3){
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
          cout << "Ship #" << id << " has been sunk\n";
          lastSunkShip = true;
          isSunk = true;
      }
      return lastSunkShip;
    }

    void aiSunkShips(){
        size--;
        if(size <= 0){
            cout << "Ai Ship #" << id << " has been sunk\n";
            isSunk = true;
        }
    }
};


//Board logic

vector<vector<int>> boardMake() {
    vector<vector<int>> board(8, vector<int>(8, -3)); // -3 for empty spaces
    return board;
}

vector<vector<int>> computerBoardMake() {
    vector<vector<int>> aiBoard(8, vector<int>(8, -3)); // -3 for empty spaces
    return aiBoard;
}


void renderBoard(vector<vector<int>> board) {
    cout << "   A B C D E F G H\n";
    for (int i = 0; i < 8; i++) {
        cout << i + 1 << " |";
        for (int j = 0; j < 8; j++) {
            if (board[i][j] == -3) {
                cout << "~ ";  // Water
            } else if (board[i][j] == -2) {
                cout << "0 ";  // Miss
            } else if (board[i][j] == -1) {
                cout << "X ";  // Hit
            } else {
                cout << board[i][j] << " ";  // Display the ship's ID
            }
        }
        cout << "\n";
    }
}

void renderGame(vector<vector<int>> board, vector<vector<int>> aiBoard) {
    cout << "#####PLAYER BOARD######      #####AI BOARD#####\n";
    cout << "   A B C D E F G H              A B C D E F G H\n";

    for (int i = 0; i < 8; i++) {
        // Render player board
        cout << i + 1 << " |";
        for (int j = 0; j < 8; j++) {
            if (board[i][j] == -3) {
                cout << "~ ";  // Water
            } else if (board[i][j] == -2) {
                cout << "0 ";  // Miss
            } else if (board[i][j] == -1) {
                cout << "X ";  // Hit
            } else {
                cout << board[i][j] << " ";  // Display the ship's ID
            }
        }

        // Spacer between boards
        cout << "          ";

        // Render computer board
        cout << i + 1 << " |";
        for (int j = 0; j < 8; j++) {
            if (aiBoard[i][j] == -2) {
                cout << "0 ";  // Miss
            } else if (aiBoard[i][j] == -1) {
                cout << "X ";  // Hit
            } else{
                cout << "~ "; // Water
            }
        }
        cout << "\n";
    }
}


//Players turns
auto playerTurn(vector<vector<int>> &aiBoard){
    system_clear();    renderGame(board, aiBoard);
    int aiShipHit = 0; //Default value;
    char x_in;
    int y_in;
    bool isValid = false;
    while(!isValid){
    cout << "#####PLAYER MOVE#####\n";
    cout << "Input coordinates of your hit: ";
    cin >> x_in >> y_in;

    int x = x_in - 'A';  // Convert letter to integer index
    int y = y_in - 1;    // Adjust for 0-based indexing

    // Ensure coordinates are within bounds
    if (x < 0 || x >= 8 || y < 0 || y >= 8) {
    cout << "Coordinates out of bounds. Please try again.\n";
    continue;
    }
    if(aiBoard[y][x] == -1 || aiBoard[y][x] == -2) {
        cout << "You already used this spot, try again\n";
        continue;
    } else if(aiBoard[y][x] != -3){
        isValid = true;
        switch(aiBoard[y][x]){
            case 2: aiShipHit = 2; break;
            case 3: aiShipHit = 3; break;
            case 4: aiShipHit = 4; break;
            case 5: aiShipHit = 5; break;
            default:
                cout << "Ai turn error\n";
        }
        aiBoard[y][x] = -1; // hit
        system_clear();        renderGame(board, aiBoard);
        cout << "#####PLAYER MOVE#####\n";
        cout << x_in << y_in << '\n';
        cout << "HIT\n";

    } else{
        isValid = true;
        aiBoard[y][x] = -2; // miss
        switch(aiBoard[y][x]){
            case 2: aiShipHit = 2; break;
            case 3: aiShipHit = 3; break;
            case 4: aiShipHit = 4; break;
            case 5: aiShipHit = 5; break;
            default:
                cout << "Ai turn error\n";
        }
        system_clear();        renderGame(board, aiBoard);
        cout << "#####PLAYER MOVE#####\n";
        cout << x_in << y_in << '\n';
        cout << "MISS\n";
    }

    }
    return aiShipHit;
}



int main() {
    bool isOver = false;
    bool playerSunk = false;
    bool aiSunk = false;
    int playerNumMoves = 0;
    int aiNumMoves = 0;
    int Last_x = -1;
    int Last_y = -1;
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
    cout << "\n";
    cout << "#####SHIP 2#####\n";
    ship_2.placeShip(board, 2);  // Ship 2 has 2 coordinates
    cout << "#####SHIP 3#####\n";
    ship_3.placeShip(board, 3);  // Ship 3 has 3 coordinates
    cout << "#####SHIP 4#####\n";
    ship_4.placeShip(board, 4);  // Ship 4 has 4 coordinates
    cout << "#####SHIP 5#####\n";
    ship_5.placeShip(board, 5);  // Ship 5 has 5 coordinates
    system_clear();
    // Render the boards
    renderGame(board, aiBoard);

    // Game loop
    while (!isOver) {
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
                cout << "AI has sunk all your ships. AI wins!\n";
                isOver = true;
                break;
            }
            Sleep(3000);  // Wait a bit before next turn

        }
        else if (turn == 0) {  // AI MOVE ######################
            bool lastSunkShip = false;  // Flag to check if any AI ship is sunk
            int shipHit;
            std::tie(shipHit, Last_x, Last_y) = aiTurn(lastSunkShip, board, Last_x, Last_y, aiNumMoves);  // Pass Last_x, Last_y
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
                cout << "Player has sunk all enemy ships. Player wins!\n";
                cout << playerNumMoves;
                isOver = true;
                break;
            }
            Sleep(3000);  // Wait a bit before next turn
        }
    }
    return 0;
}

