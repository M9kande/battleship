#pragma once
#include <iostream>
#include <algorithm>
#include "GameTypes.h"


struct Ship {
    int id = 0;
    int size = 0;
    Vec4 coordinates; // Store coordinates as pairs of (x, y)
    bool isSunk = false;
    

    Ship(int id, int size){
        this->id = id;
        this->size = size;
    }
    
    // General function to handle placing ships
    void placeShip(Board &board, int numCoords) {
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


    void placeAiShip(Board &aiBoard, int aiNumCoords){
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
            //check overlap
            if(coordinates.size() == aiNumCoords){
                bool overlap = false;
                for(auto &coord: coordinates){
                    for(int j = -1; j <= 1; j++){
                        if(aiBoard[std::clamp(coord.second + j, 0, boardSize-1)][coord.first] != empty){
                            overlap = true;
                            break;
                        }
                        if(aiBoard[coord.second][std::clamp(coord.first + j, 0, boardSize-1)] != empty){
                            overlap = true;
                            break;
                        }
                        if(aiBoard[std::clamp(coord.second + 1, 0, boardSize-1)][std::clamp(coord.first + j, 0, boardSize-1)] != empty){
                            overlap = true;
                            break;
                        }
                        if(aiBoard[std::clamp(coord.second - 1, 0, boardSize-1)][std::clamp(coord.first + j, 0, boardSize-1)] != empty){
                            overlap = true;
                            break;
                        }
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