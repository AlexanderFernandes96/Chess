#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

extern bool playerTurn; // White = true, Black = false

char** createChessBoard();
void freeChessBoard(char** board);
void resetChessBoard(char** board);
void printChessBoard(char** board);
int movePiece(char** board, const char* loc);
int checkMove(char** board, int rf, int cf, int rt, int ct);

#endif // CHESS_BOARD_H

#ifdef __cplusplus
}
#endif