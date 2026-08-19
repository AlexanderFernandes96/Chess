#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include <stdbool.h>
#include <stdint.h>

#define GRID(r,c) ((r) * 8 + (c))
#define NO_EN_PASSANT 64

typedef struct {
    char board[64]; // 8x8 chessboard represented as a 1D array
    bool playerTurn; // White = true, Black = false
    uint8_t castlingRights; // 4 bits for castling rights
    uint8_t enPassantSquare; // 0-63 index for en passant target square to move to 
} ChessBoard;

enum {
    A8 = 0, B8, C8, D8, E8, F8, G8, H8,
    A7 = 8, B7, C7, D7, E7, F7, G7, H7,
    A6 = 16, B6, C6, D6, E6, F6, G6, H6,
    A5 = 24, B5, C5, D5, E5, F5, G5, H5,
    A4 = 32, B4, C4, D4, E4, F4, G4, H4,
    A3 = 40, B3, C3, D3, E3, F3, G3, H3,
    A2 = 48, B2, C2, D2, E2, F2, G2, H2,
    A1 = 56, B1, C1, D1, E1, F1, G1, H1
};

enum { 
    MOVE_SUCCESS = 0x00, 
    MOVE_INVALID = 0x7F, 
    MOVE_QUIT = 0xFF,
    CASTLE_WHITE_KINGSIDE = 0x01, 
    CASTLE_WHITE_QUEENSIDE = 0x02, 
    CASTLE_BLACK_KINGSIDE = 0x04, 
    CASTLE_BLACK_QUEENSIDE = 0x08,
    EN_PASSANT = 0x10
}; // status for moving a piece

void resetChessBoard(ChessBoard *board);
void printChessBoard(const ChessBoard *board);
uint8_t movePiece(ChessBoard *board, uint8_t rf, uint8_t cf, uint8_t rt, uint8_t ct);
uint8_t checkMove(const ChessBoard *board, uint8_t rf, uint8_t cf, uint8_t rt, uint8_t ct);

#endif // CHESS_BOARD_H

#ifdef __cplusplus
}
#endif