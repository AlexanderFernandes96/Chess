#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

extern "C" {
#include "ChessBoard.h"
}

std::string parseInput(std::string input) {
    input.erase(
        std::remove_if(input.begin(), input.end(),
            [](unsigned char character) {
                return std::isspace(character);
            }),
        input.end());

    return input;
}

int main()
{
    // Initialize variables
    ChessBoard chessboard;
    resetChessBoard(&chessboard);
    
    uint8_t move = MOVE_SUCCESS; // Initialize move to success to enter the loop
    while(move != MOVE_QUIT) {
        if (move == MOVE_SUCCESS) { 
            system("cls"); 
            printChessBoard(&chessboard); 
        }

        std::string input;
        std::cout << (chessboard.playerTurn ? "White (lowercase)" : "Black (uppercase)") << " move: ";
        std::getline(std::cin, input);

        std::string chessMove = parseInput(input);
        if (move == MOVE_SUCCESS) 
            move = movePieceChessNotation(&chessboard, chessMove.c_str()); 
        if (move == MOVE_INVALID) { 
            std::cout << "Invalid input! Use the format: e.g., c2c4 (for a piece at c2 with a valid move at c4) or 'Q' to quit" 
                      << std::endl; 
        }

    }

    // End of the program, free the allocated memory
    return 0;
}