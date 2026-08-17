#include <stdlib.h>
#include <stdio.h>
#include "ChessBoard.h"

char** createChessBoard() {
    char** board = (char**)malloc(8 * sizeof(char*));
    for (int i = 0; i < 8; i++) {
        board[i] = (char*)malloc(8 * sizeof(char));
    }
    return board;
}

void freeChessBoard(char** board) {
    for (int i = 0; i < 8; i++) {
        free(board[i]);
    }
    free(board);
}

void resetChessBoard(char** board) {
    char bd[8][8] = {
        {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'},
        {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'}
    };
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board[i][j] = bd[i][j];
        }
    }
}

void printChessBoard(char** board) {
    printf("    a   b   c   d   e   f   g   h  \n");
    for (int i = 0; i < 8; i++) {
        printf("  +-------------------------------+\n");
        for (int j = 0; j < 10; j++) {
            if (j == 0) {
                printf("%d | ", 8-i);
            } else if (j == 9) {
                printf("%d", 8-i);
            } else {
                printf("%c | ", board[i][j-1]);
            }
        }
        printf("\n");
    }
    printf("  +-------------------------------+\n");
    printf("    a   b   c   d   e   f   g   h  \n");
}   


int movePiece(char** board, const char* loc) {

    if (loc[0] == 'Q' || loc[0] == 'q') {
        return 1; // Quit
    }

    int colFrom, rowFrom, colTo, rowTo, status = 0;
    colFrom = loc[0] - 'a';
    rowFrom = 8 - (loc[1] - '0');
    colTo = loc[2] - 'a';
    rowTo = 8 - (loc[3] - '0');

    if (rowFrom < 0 || rowFrom > 7 || colFrom < 0 || colFrom > 7 ||
       rowTo < 0 || rowTo > 7 || colTo < 0 || colTo > 7) {
        return -1; // Error
    }

    if (!(rowFrom == rowTo && colFrom == colTo)) {
        if (board[rowFrom][colFrom] != ' ') {
            board[rowTo][colTo] = board[rowFrom][colFrom];
            board[rowFrom][colFrom] = ' ';
        } else { return -1; } // Error: No piece at 'From' location
    } 

    return status; // Move successful
}