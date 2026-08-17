#include <iostream>
#include <string>
#include "ChessBoard.h"

int main()
{
    // Initialize variables
    char** board = createChessBoard();
    resetChessBoard(board);
    playerTurn = false;
    
    int move = 0;
    while(move != 1) {
        if (move == 0) { 
            playerTurn = !playerTurn; 
            system("cls"); 
            printChessBoard(board); 
        }

        std::string loc;
        std::cout << (playerTurn ? "White (lowercase)" : "Black (uppercase)") << " move: ";
        std::getline(std::cin, loc);
        loc.erase(std::remove_if(loc.begin(), loc.end(), std::isspace), loc.end()); // remove white space from the input
        move = movePiece(board, loc.c_str());
        if (move == -1) { 
            std::cout << "Invalid move! Use the format: e.g., c2c4 (for a piece at c2 with a valid move at c4) or 'Q' to quit" 
                      << std::endl; 
        }

    }

    // End of the program, free the allocated memory
    freeChessBoard(board);
    return 0;
}