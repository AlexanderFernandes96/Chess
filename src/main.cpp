#include <iostream>
#include <string>
#include "ChessBoard.h"

uint8_t parseInput(std::string loc, uint8_t &rf, uint8_t &cf, uint8_t &rt, uint8_t &ct) {

    loc.erase(std::remove_if(loc.begin(), loc.end(), std::isspace), loc.end()); // remove white space from the input

    if (loc[0] == 'Q' || loc[0] == 'q') return MOVE_QUIT;
    if (loc.length() != 4) return MOVE_INVALID;   
    if (loc[0] < 'a' || loc[0] > 'h' || loc[1] < '1' || loc[1] > '8' ||
        loc[2] < 'a' || loc[2] > 'h' || loc[3] < '1' || loc[3] > '8') {
        return MOVE_INVALID; // Error: invalid input
    }

    cf = loc[0] - 'a';
    rf = 8 - (loc[1] - '0');
    ct = loc[2] - 'a';
    rt = 8 - (loc[3] - '0');
    return MOVE_SUCCESS;
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

        std::string loc;
        std::cout << (chessboard.playerTurn ? "White (lowercase)" : "Black (uppercase)") << " move: ";
        std::getline(std::cin, loc);
        uint8_t rf, cf, rt, ct; // row and column for from/to squares
        move = parseInput(loc, rf, cf, rt, ct);
        if (move == MOVE_SUCCESS) move = movePiece(&chessboard, rf, cf, rt, ct); 
        if (move == MOVE_INVALID) { 
            std::cout << "Invalid input! Use the format: e.g., c2c4 (for a piece at c2 with a valid move at c4) or 'Q' to quit" 
                      << std::endl; 
        }

    }

    // End of the program, free the allocated memory
    return 0;
}