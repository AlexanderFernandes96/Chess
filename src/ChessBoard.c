#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "ChessBoard.h"

bool playerTurn;

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
    if (playerTurn) { // white's turn
        printf("    a   b   c   d   e   f   g   h  \n");
        for (int i = 0; i < 8; i++) {
            if (i % 2 == 0) {
                printf("  +===+---+===+---+===+---+===+---+\n");
            } else {
                printf("  +---+===+---+===+---+===+---+===+\n");
            }
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
        printf("  +===+---+===+---+===+---+===+---+\n");
        printf("    a   b   c   d   e   f   g   h  \n");
    } else { // rotate board for black's turn
        printf("    h   g   f   e   d   c   b   a  \n");
        for (int i = 7; i >= 0; i--) {
            if (i % 2 == 0) {
                printf("  +---+===+---+===+---+===+---+===+\n");
            } else {
                printf("  +===+---+===+---+===+---+===+---+\n");
            }
            for (int j = 9; j >= 0; j--) {
                if (j == 0) {
                    printf("| %d", 8-i);
                } else if (j == 9) {
                    printf("%d ", 8-i);
                } else {
                    printf("| %c ", board[i][j-1]);
                }
            }
            printf("\n");
        }
        printf("  +===+---+===+---+===+---+===+---+\n");
        printf("    h   g   f   e   d   c   b   a  \n");
    }
}   


int movePiece(char** board, const char* loc) {

    if (loc[0] == 'Q' || loc[0] == 'q') {
        return 1; // Quit
    }

    int cf, rf, ct, rt, status = -1;
    cf = loc[0] - 'a';
    rf = 8 - (loc[1] - '0');
    ct = loc[2] - 'a';
    rt = 8 - (loc[3] - '0');

    status = checkMove(board, rf, cf, rt, ct);
    if (status == 0) {
        // Move the piece
        board[rt][ct] = board[rf][cf];
        board[rf][cf] = ' ';
    } else if (status == 10) { // white kingside castling
        board[7][6] = 'k';
        board[7][5] = 'r';
        board[7][4] = ' ';
        board[7][7] = ' ';
        status = 0;
    } else if (status == 11) { // white queenside castling
        board[7][2] = 'k';
        board[7][3] = 'r';
        board[7][4] = ' ';
        board[7][0] = ' ';
        status = 0;
    } else if (status == 12) { // black kingside castling
        board[0][6] = 'K';
        board[0][5] = 'R';
        board[0][4] = ' ';
        board[0][7] = ' ';
        status = 0;
    } else if (status == 13) { // black queenside castling
        board[0][2] = 'K';
        board[0][3] = 'R';
        board[0][4] = ' ';
        board[0][0] = ' ';
        status = 0;
    }

    return status;
}


int checkMove(char** board, int rf, int cf, int rt, int ct) {
    int status = -1; // Default to error
    int kk = 10, kq = 11, Kk = 12, Kq = 13; // Can start off by castling kingside/queenside for white and black

    if (rf < 0 || rf > 7 || cf < 0 || cf > 7 ||
       rt < 0 || rt > 7 || ct < 0 || ct > 7) {
        return -1; // Error: out of bounds
    }

    if (!(rf == rt && cf == ct)) { // from/to are not the same location
        char piece = board[rf][cf];
        switch (piece) { // Check piece moves
            case 'p': // white pawn
                if(playerTurn) { 
                    if (rt == rf - 1 && ct == cf && board[rt][ct] == ' ') {
                        status = 0; // move one space
                    } else if (rt == rf - 2 && ct == cf && rf == 6 && board[rt][ct] == ' ' && board[rt + 1][ct] == ' ') {
                        status = 0; // move two spaces from starting position
                    } else if (rt == rf - 1 && (ct == cf - 1 || ct == cf + 1) && board[rt][ct] >= 'A' && board[rt][ct] <= 'Z') {
                        status = 0; // capture diagonally
                    }
                }
                break;
            
            case 'P': // black pawn
                if(!playerTurn) { 
                    if (rt == rf + 1 && ct == cf && board[rt][ct] == ' ') {
                        status = 0; // move one space
                    } else if (rt == rf + 2 && ct == cf && rf == 1 && board[rt][ct] == ' ' && board[rt - 1][ct] == ' ') {
                        status = 0; // move two spaces from starting position
                    } else if (rt == rf + 1 && (ct == cf - 1 || ct == cf + 1) && board[rt][ct] >= 'a' && board[rt][ct] <= 'z') {
                        status = 0; // capture diagonally
                    }
                }
                break;
            
            case 'n': // white knight
                if(playerTurn) { 
                    if ((abs(rt - rf) == 2 && abs(ct - cf) == 1) || (abs(rt - rf) == 1 && abs(ct - cf) == 2)) {
                        if (!(board[rt][ct] >= 'a' && board[rt][ct] <= 'z')) status = 0; 
                    }
                }
                break;
                
            case 'N': // black knight
                if(!playerTurn) { 
                    if ((abs(rt - rf) == 2 && abs(ct - cf) == 1) || (abs(rt - rf) == 1 && abs(ct - cf) == 2)) {
                        if (!(board[rt][ct] >= 'A' && board[rt][ct] <= 'Z')) status = 0; 
                    }
                }
                break;

            case 'b': // white bishop
                if(playerTurn) { 
                    if (abs(rt - rf) == abs(ct - cf)) {
                        int mx = abs(rt - rf);
                        int r = (rt - rf) / mx, c = (ct - cf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (board[rf + i*r][cf + i*c] != ' ') {
                                return -1; // Error: path blocked
                            }
                        }
                        if (!(board[rt][ct] >= 'a' && board[rt][ct] <= 'z')) status = 0; 
                    }
                }
                break;
                
            case 'B': // black bishop
                if(!playerTurn) { 
                    if (abs(rt - rf) == abs(ct - cf)) {
                        int mx = abs(rt - rf);
                        int r = (rt - rf) / mx, c = (ct - cf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (board[rf + i*r][cf + i*c] != ' ') {
                                return -1; // Error: path blocked
                            }
                        }
                        if (!(board[rt][ct] >= 'A' && board[rt][ct] <= 'Z')) status = 0; 
                    }
                }
                break;
            
            case 'r': // white rook
                if(playerTurn) { 
                    if (rt == rf) {
                        int mx = abs(ct - cf);
                        int c = (ct - cf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (board[rf][cf + i*c] != ' ') {
                                return -1; // Error: path blocked
                            }
                        }
                        if (!(board[rt][ct] >= 'a' && board[rt][ct] <= 'z')) {
                            status = 0; 
                            if (rf == 7 && cf == 0) kq = -1; // white queenside no castling
                            if (rf == 7 && cf == 7) kk = -1; // white kingside no castling
                        }
                    } else if(ct == cf) {
                        int mx = abs(rt - rf);
                        int r = (rt - rf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (board[rf + i*r][cf] != ' ') {
                                return -1; // Error: path blocked
                            }
                        }
                        if (!(board[rt][ct] >= 'a' && board[rt][ct] <= 'z')) {
                            status = 0; 
                            if (rf == 7 && cf == 0) kq = -1; // white queenside no castling
                            if (rf == 7 && cf == 7) kk = -1; // white kingside no castling
                        }
                    }
                }
                break;
            
            case 'R': // black rook
                if(!playerTurn) { 
                    if (rt == rf) {
                        int mx = abs(ct - cf);
                        int c = (ct - cf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (board[rf][cf + i*c] != ' ') {
                                return -1; // Error: path blocked
                            }
                        }
                        if (!(board[rt][ct] >= 'A' && board[rt][ct] <= 'Z')) {
                            status = 0; 
                            if (rf == 0 && cf == 0) Kq = -1; // black queenside no castling
                            if (rf == 0 && cf == 7) Kk = -1; // black kingside no castling
                        }
                    } else if(ct == cf) {
                        int mx = abs(rt - rf);
                        int r = (rt - rf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (board[rf + i*r][cf] != ' ') {
                                return -1; // Error: path blocked
                            }
                        }
                        if (!(board[rt][ct] >= 'A' && board[rt][ct] <= 'Z')) {
                            status = 0; 
                            if (rf == 0 && cf == 0) Kq = -1; // black queenside no castling
                            if (rf == 0 && cf == 7) Kk = -1; // black kingside no castling
                        }
                    }
                }
                break;

            case 'q': // white queen
                if(playerTurn) { 
                    if (abs(rt - rf) == abs(ct - cf)) {
                        int mx = abs(rt - rf);
                        int r = (rt - rf) / mx, c = (ct - cf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (board[rf + i*r][cf + i*c] != ' ') {
                                return -1; // Error: path blocked
                            }
                        }
                        if (!(board[rt][ct] >= 'a' && board[rt][ct] <= 'z')) status = 0; 
                    } else if (rt == rf) {
                        int mx = abs(ct - cf);
                        int c = (ct - cf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (board[rf][cf + i*c] != ' ') {
                                return -1; // Error: path blocked
                            }
                        }
                        if (!(board[rt][ct] >= 'a' && board[rt][ct] <= 'z')) status = 0; 
                    } else if(ct == cf) {
                        int mx = abs(rt - rf);
                        int r = (rt - rf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (board[rf + i*r][cf] != ' ') {
                                return -1; // Error: path blocked
                            }
                        }
                        if (!(board[rt][ct] >= 'a' && board[rt][ct] <= 'z')) status = 0; 
                    }
                }
                break;
            
            case 'Q': // black queen
                if(!playerTurn) { 
                    if (rt - rf == ct - cf) {
                        int mx = abs(rt - rf);
                        int r = (rt - rf) / mx, c = (ct - cf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (board[rf + i*r][cf + i*c] != ' ') {
                                return -1; // Error: path blocked
                            }
                        }
                        if (!(board[rt][ct] >= 'A' && board[rt][ct] <= 'Z')) status = 0; 
                    } else if (rt == rf) {
                        int mx = abs(ct - cf);
                        int c = (ct - cf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (board[rf][cf + i*c] != ' ') {
                                return -1; // Error: path blocked
                            }
                        }
                        if (!(board[rt][ct] >= 'A' && board[rt][ct] <= 'Z')) status = 0; 
                    } else if(ct == cf) {
                        int mx = abs(rt - rf);
                        int r = (rt - rf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (board[rf + i*r][cf] != ' ') {
                                return -1; // Error: path blocked
                            }
                        }
                        if (!(board[rt][ct] >= 'A' && board[rt][ct] <= 'Z')) status = 0; 
                    }
                }
                break;
            
            case 'k': // white king
                if(playerTurn) { 
                    if (abs(rt - rf) <= 1 && abs(ct - cf) <= 1) {
                        if (!(board[rt][ct] >= 'a' && board[rt][ct] <= 'z')) status = 0; 
                    } else if (rf == 7 && cf == 4 && rt == 7 && ct == 6 && board[7][5] == ' ' && board[7][6] == ' ' && board[7][7] == 'r') {
                        status = kk; // kingside castling
                    } else if (rf == 7 && cf == 4 && rt == 7 && ct == 2 && board[7][1] == ' ' && board[7][2] == ' ' && board[7][3] == ' ' && board[7][0] == 'r') {
                        status = kq; // queenside castling
                    }
                }
                break;
            
            case 'K': // black king
                if(!playerTurn) { 
                    if (abs(rt - rf) <= 1 && abs(ct - cf) <= 1) {
                        if (!(board[rt][ct] >= 'A' && board[rt][ct] <= 'Z')) status = 0; 
                    } else if (rf == 0 && cf == 4 && rt == 0 && ct == 6 && board[0][5] == ' ' && board[0][6] == ' ' && board[0][7] == 'R') {
                        status = Kk; // kingside castling
                    } else if (rf == 0 && cf == 4 && rt == 0 && ct == 2 && board[0][1] == ' ' && board[0][2] == ' ' && board[0][3] == ' ' && board[0][0] == 'R') {
                        status = Kq; // queenside castling
                    }
                }
                break;

            default:
                status = -1; // Error: invalid piece
                break;
        }
    }
    return status;
}