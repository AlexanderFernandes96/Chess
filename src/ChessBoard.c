#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ChessBoard.h"

void resetChessBoard(ChessBoard *chessboard) {
    
    if (chessboard == NULL) {
        fprintf(stderr, "Error: chessboard pointer is NULL\n");
        exit(EXIT_FAILURE);
    }

    chessboard->playerTurn = true; // White starts first
    chessboard->castlingRights = 0xF; // All castling rights enabled
    chessboard->enPassantSquare = NO_EN_PASSANT; // No en passant target
    
    static const char bd[64] = {
        'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R',
        'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
        'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'
    };

    memcpy(chessboard->board, bd, sizeof(bd));
}

void printChessBoard(const ChessBoard *chessboard) {
    if (chessboard == NULL) {
        printf("Error: chessboard pointer is NULL\n");
        return;
    }

    if (chessboard->playerTurn) { // white's turn
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
                    printf("%c | ", chessboard->board[GRID(i,j-1)]);
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
                    printf("| %c ", chessboard->board[GRID(i, j-1)]);
                }
            }
            printf("\n");
        }
        printf("  +===+---+===+---+===+---+===+---+\n");
        printf("    h   g   f   e   d   c   b   a  \n");
    }
}   

uint8_t movePieceChessNotation(ChessBoard *board, const char *chessMove)
{
    if (board == NULL || chessMove == NULL) 
        return MOVE_INVALID;
    if (chessMove[0] == 'Q' || chessMove[0] == 'q') 
        return MOVE_QUIT;
    if (strlen(chessMove) != 4)
       return MOVE_INVALID;
    if (chessMove[0] < 'a' || chessMove[0] > 'h' || chessMove[1] < '1' || chessMove[1] > '8' || 
        chessMove[2] < 'a' || chessMove[2] > 'h' || chessMove[3] < '1' || chessMove[3] > '8') 
        return MOVE_INVALID;

    uint8_t fromFile = (uint8_t)(chessMove[0] - 'a');
    uint8_t fromRank = (uint8_t)(8 - (chessMove[1] - '0'));
    uint8_t toFile = (uint8_t)(chessMove[2] - 'a');
    uint8_t toRank = (uint8_t)(8 - (chessMove[3] - '0'));

    return movePiece(board, fromRank, fromFile, toRank, toFile);
}

uint8_t movePiece(ChessBoard *chessboard, uint8_t rf, uint8_t cf, uint8_t rt, uint8_t ct) {
    if (chessboard == NULL) return MOVE_INVALID; 

    uint8_t status = checkMove(chessboard, rf, cf, rt, ct);
    bool isDoublePawnMove = false;
    if (status == MOVE_SUCCESS) {
        // Update castling rights
        char piece = chessboard->board[GRID(rf, cf)];
        switch (piece) {
            case 'p':
                    if (rt == rf - 2 && ct == cf && rf == 6 && chessboard->board[GRID(rt, ct)] == ' ' && chessboard->board[GRID(rt + 1, ct)] == ' ') {
                        chessboard->enPassantSquare = GRID(5, ct); // set en passant square
                        isDoublePawnMove = true;
                    }
                    
                break;
            case 'P':
                    if (rt == rf + 2 && ct == cf && rf == 1 && chessboard->board[GRID(rt, ct)] == ' ' && chessboard->board[GRID(rt - 1, ct)] == ' ') {
                        chessboard->enPassantSquare = GRID(2, ct); // set en passant square
                        isDoublePawnMove = true;
                    }
                break;
            case 'k':
                chessboard->castlingRights &= ~(CASTLE_WHITE_KINGSIDE | CASTLE_WHITE_QUEENSIDE);
                break;
            case 'K':
                chessboard->castlingRights &= ~(CASTLE_BLACK_KINGSIDE | CASTLE_BLACK_QUEENSIDE);
                break;
            case 'r':
                if (rf == 7 && cf == 0) chessboard->castlingRights &= ~CASTLE_WHITE_QUEENSIDE;
                if (rf == 7 && cf == 7) chessboard->castlingRights &= ~CASTLE_WHITE_KINGSIDE;
                break;
            case 'R':
                if (rf == 0 && cf == 0) chessboard->castlingRights &= ~CASTLE_BLACK_QUEENSIDE;
                if (rf == 0 && cf == 7) chessboard->castlingRights &= ~CASTLE_BLACK_KINGSIDE;
                break;
        }
        
        // Move the piece normally
        chessboard->board[GRID(rt, ct)] = chessboard->board[GRID(rf, cf)];
        chessboard->board[GRID(rf, cf)] = ' ';
        
    } else if (status == CASTLE_WHITE_KINGSIDE) { // white kingside castling
        chessboard->board[62] = 'k'; // 7 * 8 + 6
        chessboard->board[61] = 'r'; // 7 * 8 + 5
        chessboard->board[60] = ' '; // 7 * 8 + 4
        chessboard->board[63] = ' '; // 7 * 8 + 7
        chessboard->castlingRights &= ~CASTLE_WHITE_KINGSIDE; // Remove castling right
        status = MOVE_SUCCESS;
    } else if (status == CASTLE_WHITE_QUEENSIDE) { // white queenside castling
        chessboard->board[58] = 'k'; // 7 * 8 + 2
        chessboard->board[59] = 'r'; // 7 * 8 + 3
        chessboard->board[60] = ' '; // 7 * 8 + 4
        chessboard->board[56] = ' '; // 7 * 8 + 0
        chessboard->castlingRights &= ~CASTLE_WHITE_QUEENSIDE;
        status = MOVE_SUCCESS;
    } else if (status == CASTLE_BLACK_KINGSIDE) { // black kingside castling
        chessboard->board[6] = 'K'; // 0 * 8 + 6
        chessboard->board[5] = 'R'; // 0 * 8 + 5
        chessboard->board[4] = ' '; // 0 * 8 + 4
        chessboard->board[7] = ' '; // 0 * 8 + 7
        chessboard->castlingRights &= ~CASTLE_BLACK_KINGSIDE;
        status = MOVE_SUCCESS;
    } else if (status == CASTLE_BLACK_QUEENSIDE) { // black queenside castling
        chessboard->board[2] = 'K'; // 0 * 8 + 2
        chessboard->board[3] = 'R'; // 0 * 8 + 3
        chessboard->board[4] = ' '; // 0 * 8 + 4
        chessboard->board[0] = ' '; // 0 * 8 + 0
        chessboard->castlingRights &= ~CASTLE_BLACK_QUEENSIDE;
        status = MOVE_SUCCESS;
    } else if (status == EN_PASSANT) {
        // Move the piece and en passant capture
        chessboard->board[GRID(rt, ct)] = chessboard->board[GRID(rf, cf)];
        chessboard->board[GRID(rf, cf)] = ' ';
        chessboard->board[chessboard->enPassantSquare + (chessboard->playerTurn?8:-8)] = ' ';
        status = MOVE_SUCCESS;
    }

    if (status == MOVE_SUCCESS) {
        chessboard->playerTurn = !chessboard->playerTurn; // Switch turns
        if (!isDoublePawnMove && chessboard->enPassantSquare != NO_EN_PASSANT) chessboard->enPassantSquare = NO_EN_PASSANT; 
        // Remove castling rights after a rook moves or is captured
        if ((chessboard->castlingRights & CASTLE_WHITE_QUEENSIDE) == CASTLE_WHITE_QUEENSIDE) if (chessboard->board[A1] != 'r') chessboard->castlingRights &= ~CASTLE_WHITE_QUEENSIDE;
        if ((chessboard->castlingRights & CASTLE_WHITE_KINGSIDE) == CASTLE_WHITE_KINGSIDE) if (chessboard->board[H1] != 'r') chessboard->castlingRights &= ~CASTLE_WHITE_KINGSIDE;
        if ((chessboard->castlingRights & CASTLE_BLACK_QUEENSIDE) == CASTLE_BLACK_QUEENSIDE) if (chessboard->board[A8] != 'R') chessboard->castlingRights &= ~CASTLE_BLACK_QUEENSIDE;
        if ((chessboard->castlingRights & CASTLE_BLACK_KINGSIDE) == CASTLE_BLACK_KINGSIDE) if (chessboard->board[H8] != 'R') chessboard->castlingRights &= ~CASTLE_BLACK_KINGSIDE;
    }
    return status;
}


uint8_t checkMove(const ChessBoard *chessboard, uint8_t rf, uint8_t cf, uint8_t rt, uint8_t ct) {
    if (chessboard == NULL) return MOVE_INVALID; 

    uint8_t status = MOVE_INVALID; // Default to error

    if (rf > 7 || cf > 7 || rt > 7 || ct > 7) return MOVE_INVALID; // Error: out of bounds

    if (!(rf == rt && cf == ct)) { // from/to are not the same location
        char piece = chessboard->board[GRID(rf, cf)];
        switch (piece) { // Check piece moves
            case 'p': // white pawn
                if(chessboard->playerTurn) { 
                    if (rt == rf - 1 && ct == cf && chessboard->board[GRID(rt, ct)] == ' ') {
                        status = MOVE_SUCCESS; // move one space
                    } else if (rt == rf - 2 && ct == cf && rf == 6 && chessboard->board[GRID(rt, ct)] == ' ' && chessboard->board[GRID(rt + 1, ct)] == ' ') {
                        status = MOVE_SUCCESS; // move two spaces from starting position
                    } else if (rt == rf - 1 && (ct == cf - 1 || ct == cf + 1) && chessboard->board[GRID(rt, ct)] >= 'A' && chessboard->board[GRID(rt, ct)] <= 'Z') {
                        status = MOVE_SUCCESS; // capture diagonally
                    } else if (rt == rf - 1 && (ct == cf - 1 || ct == cf + 1) && chessboard->enPassantSquare == rt * 8 + ct && chessboard->board[chessboard->enPassantSquare + 8] == 'P') {
                        status = EN_PASSANT; // en passant capture
                    }
                }
                break;
            
            case 'P': // black pawn
                if(!chessboard->playerTurn) { 
                    if (rt == rf + 1 && ct == cf && chessboard->board[GRID(rt, ct)] == ' ') {
                        status = MOVE_SUCCESS; // move one space
                    } else if (rt == rf + 2 && ct == cf && rf == 1 && chessboard->board[GRID(rt, ct)] == ' ' && chessboard->board[GRID(rt - 1, ct)] == ' ') {
                        status = MOVE_SUCCESS; // move two spaces from starting position
                    } else if (rt == rf + 1 && (ct == cf - 1 || ct == cf + 1) && chessboard->board[GRID(rt, ct)] >= 'a' && chessboard->board[GRID(rt, ct)] <= 'z') {
                        status = MOVE_SUCCESS; // capture diagonally
                    } else if (rt == rf + 1 && (ct == cf - 1 || ct == cf + 1) && chessboard->enPassantSquare == rt * 8 + ct && chessboard->board[chessboard->enPassantSquare - 8] == 'p') {
                        status = EN_PASSANT; // en passant capture
                    }
                }
                break;
            
            case 'n': // white knight
                if(chessboard->playerTurn) { 
                    if ((abs(rt - rf) == 2 && abs(ct - cf) == 1) || (abs(rt - rf) == 1 && abs(ct - cf) == 2)) {
                        if (!(chessboard->board[GRID(rt, ct)] >= 'a' && chessboard->board[GRID(rt, ct)] <= 'z')) status = MOVE_SUCCESS; 
                    }
                }
                break;
                
            case 'N': // black knight
                if(!chessboard->playerTurn) { 
                    if ((abs(rt - rf) == 2 && abs(ct - cf) == 1) || (abs(rt - rf) == 1 && abs(ct - cf) == 2)) {
                        if (!(chessboard->board[GRID(rt, ct)] >= 'A' && chessboard->board[GRID(rt, ct)] <= 'Z')) status = MOVE_SUCCESS; 
                    }
                }
                break;

            case 'b': // white bishop
                if(chessboard->playerTurn) { 
                    if (abs(rt - rf) == abs(ct - cf)) {
                        int mx = abs(rt - rf);
                        int r = (rt - rf) / mx, c = (ct - cf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (chessboard->board[GRID(rf + i*r, cf + i*c)] != ' ') {
                                return MOVE_INVALID; // Error: path blocked
                            }
                        }
                        if (!(chessboard->board[GRID(rt, ct)] >= 'a' && chessboard->board[GRID(rt, ct)] <= 'z')) status = MOVE_SUCCESS; 
                    }
                }
                break;
                
            case 'B': // black bishop
                if(!chessboard->playerTurn) { 
                    if (abs(rt - rf) == abs(ct - cf)) {
                        int mx = abs(rt - rf);
                        int r = (rt - rf) / mx, c = (ct - cf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (chessboard->board[GRID(rf + i*r, cf + i*c)] != ' ') {
                                return MOVE_INVALID; // Error: path blocked
                            }
                        }
                        if (!(chessboard->board[GRID(rt, ct)] >= 'A' && chessboard->board[GRID(rt, ct)] <= 'Z')) status = MOVE_SUCCESS; 
                    }
                }
                break;
            
            case 'r': // white rook
                if(chessboard->playerTurn) { 
                    if (rt == rf) {
                        int mx = abs(ct - cf);
                        int c = (ct - cf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (chessboard->board[GRID(rf, cf + i*c)] != ' ') {
                                return MOVE_INVALID; // Error: path blocked
                            }
                        }
                        if (!(chessboard->board[GRID(rt, ct)] >= 'a' && chessboard->board[GRID(rt, ct)] <= 'z')) {
                            status = MOVE_SUCCESS; 
                        }
                    } else if(ct == cf) {
                        int mx = abs(rt - rf);
                        int r = (rt - rf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (chessboard->board[GRID(rf + i*r, cf)] != ' ') {
                                return MOVE_INVALID; // Error: path blocked
                            }
                        }
                        if (!(chessboard->board[GRID(rt, ct)] >= 'a' && chessboard->board[GRID(rt, ct)] <= 'z')) {
                            status = MOVE_SUCCESS; 
                        }
                    }
                }
                break;
            
            case 'R': // black rook
                if(!chessboard->playerTurn) { 
                    if (rt == rf) {
                        int mx = abs(ct - cf);
                        int c = (ct - cf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (chessboard->board[GRID(rf, cf + i*c)] != ' ') {
                                return MOVE_INVALID; // Error: path blocked
                            }
                        }
                        if (!(chessboard->board[GRID(rt, ct)] >= 'A' && chessboard->board[GRID(rt, ct)] <= 'Z')) {
                            status = MOVE_SUCCESS; 
                        }
                    } else if(ct == cf) {
                        int mx = abs(rt - rf);
                        int r = (rt - rf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (chessboard->board[GRID(rf + i*r, cf)] != ' ') {
                                return MOVE_INVALID; // Error: path blocked
                            }
                        }
                        if (!(chessboard->board[GRID(rt, ct)] >= 'A' && chessboard->board[GRID(rt, ct)] <= 'Z')) {
                            status = MOVE_SUCCESS; 
                        }
                    }
                }
                break;

            case 'q': // white queen
                if(chessboard->playerTurn) { 
                    if (abs(rt - rf) == abs(ct - cf)) {
                        int mx = abs(rt - rf);
                        int r = (rt - rf) / mx, c = (ct - cf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (chessboard->board[GRID(rf + i*r, cf + i*c)] != ' ') {
                                return MOVE_INVALID; // Error: path blocked
                            }
                        }
                        if (!(chessboard->board[GRID(rt, ct)] >= 'a' && chessboard->board[GRID(rt, ct)] <= 'z')) status = MOVE_SUCCESS; 
                    } else if (rt == rf) {
                        int mx = abs(ct - cf);
                        int c = (ct - cf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (chessboard->board[GRID(rf, cf + i*c)] != ' ') {
                                return MOVE_INVALID; // Error: path blocked
                            }
                        }
                        if (!(chessboard->board[GRID(rt, ct)] >= 'a' && chessboard->board[GRID(rt, ct)] <= 'z')) status = MOVE_SUCCESS; 
                    } else if(ct == cf) {
                        int mx = abs(rt - rf);
                        int r = (rt - rf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (chessboard->board[GRID(rf + i*r, cf)] != ' ') {
                                return MOVE_INVALID; // Error: path blocked
                            }
                        }
                        if (!(chessboard->board[GRID(rt, ct)] >= 'a' && chessboard->board[GRID(rt, ct)] <= 'z')) status = MOVE_SUCCESS; 
                    }
                }
                break;
            
            case 'Q': // black queen
                if(!chessboard->playerTurn) { 
                    if (abs(rt - rf) == abs(ct - cf)) {
                        int mx = abs(rt - rf);
                        int r = (rt - rf) / mx, c = (ct - cf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (chessboard->board[GRID(rf + i*r, cf + i*c)] != ' ') {
                                return MOVE_INVALID; // Error: path blocked
                            }
                        }
                        if (!(chessboard->board[GRID(rt, ct)] >= 'A' && chessboard->board[GRID(rt, ct)] <= 'Z')) status = MOVE_SUCCESS; 
                    } else if (rt == rf) {
                        int mx = abs(ct - cf);
                        int c = (ct - cf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (chessboard->board[GRID(rf, cf + i*c)] != ' ') {
                                return MOVE_INVALID; // Error: path blocked
                            }
                        }
                        if (!(chessboard->board[GRID(rt, ct)] >= 'A' && chessboard->board[GRID(rt, ct)] <= 'Z')) status = MOVE_SUCCESS; 
                    } else if(ct == cf) {
                        int mx = abs(rt - rf);
                        int r = (rt - rf) / mx;
                        for (int i = 1; i < mx; i++) {
                            if (chessboard->board[GRID(rf + i*r, cf)] != ' ') {
                                return MOVE_INVALID; // Error: path blocked
                            }
                        }
                        if (!(chessboard->board[GRID(rt, ct)] >= 'A' && chessboard->board[GRID(rt, ct)] <= 'Z')) status = MOVE_SUCCESS; 
                    }
                }
                break;
            
            case 'k': // white king
                if(chessboard->playerTurn) { 
                    if (abs(rt - rf) <= 1 && abs(ct - cf) <= 1) {
                        if (!(chessboard->board[GRID(rt, ct)] >= 'a' && chessboard->board[GRID(rt, ct)] <= 'z')) status = MOVE_SUCCESS; 
                    } else if (rf == 7 && cf == 4 && rt == 7 && ct == 6 && chessboard->board[E1] == 'k' && chessboard->board[F1] == ' ' && chessboard->board[G1] == ' ' && chessboard->board[H1] == 'r') {
                        if ((chessboard->castlingRights & CASTLE_WHITE_KINGSIDE) == CASTLE_WHITE_KINGSIDE) status = CASTLE_WHITE_KINGSIDE; // kingside castling
                    } else if (rf == 7 && cf == 4 && rt == 7 && ct == 2 && chessboard->board[E1] == 'k' && chessboard->board[D1] == ' ' && chessboard->board[C1] == ' ' && chessboard->board[B1] == ' ' && chessboard->board[A1] == 'r') {
                        if ((chessboard->castlingRights & CASTLE_WHITE_QUEENSIDE) == CASTLE_WHITE_QUEENSIDE) status = CASTLE_WHITE_QUEENSIDE; // queenside castling
                    }
                }
                break;
            
            case 'K': // black king
                if(!chessboard->playerTurn) { 
                    if (abs(rt - rf) <= 1 && abs(ct - cf) <= 1) {
                        if (!(chessboard->board[GRID(rt, ct)] >= 'A' && chessboard->board[GRID(rt, ct)] <= 'Z')) status = MOVE_SUCCESS; 
                    } else if (rf == 0 && cf == 4 && rt == 0 && ct == 6 && chessboard->board[E8] == 'K' && chessboard->board[F8] == ' ' && chessboard->board[G8] == ' ' && chessboard->board[H8] == 'R') {
                        if ((chessboard->castlingRights & CASTLE_BLACK_KINGSIDE) == CASTLE_BLACK_KINGSIDE) status = CASTLE_BLACK_KINGSIDE; // kingside castling
                    } else if (rf == 0 && cf == 4 && rt == 0 && ct == 2 && chessboard->board[E8] == 'K' && chessboard->board[D8] == ' ' && chessboard->board[C8] == ' ' && chessboard->board[B8] == ' ' && chessboard->board[A8] == 'R') {
                        if ((chessboard->castlingRights & CASTLE_BLACK_QUEENSIDE) == CASTLE_BLACK_QUEENSIDE) status = CASTLE_BLACK_QUEENSIDE; // queenside castling
                    }
                }
                break;

            default:
                status = MOVE_INVALID; // Error: invalid piece
                break;
        }
    }
    return status;
}