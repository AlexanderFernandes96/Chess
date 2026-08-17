#include <iostream>
#include "ChessBoard.h"

int main()
{
    // Initialize variables
    char** board = createChessBoard();
    resetChessBoard(board);
    printChessBoard(board);

    // End of the program, free the allocated memory
    freeChessBoard(board);
    return 0;
}