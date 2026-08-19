#include <cassert>
#include <cstdio>

extern "C" {
#include "ChessBoard.h"
}

#define RUN_TEST(test) \
    do { \
        test(); \
        std::printf("[PASS] %s\n", #test); \
    } while (false)

static ChessBoard emptyBoard(bool whiteToMove)
{
    ChessBoard board{};

    for (char &square : board.board) {
        square = ' ';
    }

    board.playerTurn = whiteToMove;
    board.castlingRights = 0;
    board.enPassantSquare = NO_EN_PASSANT;
    return board;
}

static void testResetBoard()
{
    ChessBoard board{};
    resetChessBoard(&board);

    assert(board.board[A1] == 'r');
    assert(board.board[E1] == 'k');
    assert(board.board[A8] == 'R');
    assert(board.board[E8] == 'K');
    assert(board.playerTurn);
    assert(board.castlingRights == 0xF);
    assert(board.enPassantSquare == NO_EN_PASSANT);
}

static void testWhitePawnMoves()
{
    ChessBoard board = emptyBoard(true);
    board.board[GRID(6, 4)] = 'p';
    board.board[E1] = 'k';
    board.board[E8] = 'K';

    assert(checkMove(&board, 6, 4, 5, 4) == MOVE_SUCCESS);
    assert(checkMove(&board, 6, 4, 4, 4) == MOVE_SUCCESS);
    assert(checkMove(&board, 6, 4, 6, 5) == MOVE_INVALID);
}

static void testKnightCanJump()
{
    ChessBoard board = emptyBoard(true);
    board.board[GRID(7, 1)] = 'n';
    board.board[E1] = 'k';
    board.board[E8] = 'K';

    assert(checkMove(&board, 7, 1, 5, 2) == MOVE_SUCCESS);
}

static void testRookPathBlocked()
{
    ChessBoard board = emptyBoard(true);
    board.board[GRID(7, 0)] = 'r';
    board.board[GRID(6, 0)] = 'p';
    board.board[E1] = 'k';
    board.board[E8] = 'K';

    assert(checkMove(&board, 7, 0, 5, 0) == MOVE_INVALID);
}

static void testMoveUpdatesBoard()
{
    ChessBoard board = emptyBoard(true);
    board.board[E2] = 'p';
    board.board[E1] = 'k';
    board.board[E8] = 'K';

    assert(movePieceChessNotation(&board, "e2e4") == MOVE_SUCCESS);
    assert(board.board[E2] == ' ');
    assert(board.board[E4] == 'p');
    assert(!board.playerTurn);
    assert(board.enPassantSquare == E3);
}

static void testInvalidCoordinates()
{
    ChessBoard board = emptyBoard(true);
    board.board[E1] = 'k';
    board.board[E8] = 'K';

    assert(checkMove(&board, 8, 0, 7, 0) == MOVE_INVALID);
    assert(checkMove(&board, 7, 0, 8, 0) == MOVE_INVALID);
}

static void testWhitePieceMoves()
{
    ChessBoard board = emptyBoard(true);
    board.board[E8] = 'K';

    board.board[GRID(4, 4)] = 'b'; // bishop e4
    assert(checkMove(&board, 4, 4, 2, 2) == MOVE_SUCCESS);
    assert(checkMove(&board, 4, 4, 4, 6) == MOVE_INVALID);

    board.board[GRID(4, 4)] = 'r'; // rook e4
    assert(checkMove(&board, 4, 4, 4, 7) == MOVE_SUCCESS);
    assert(checkMove(&board, 4, 4, 1, 4) == MOVE_SUCCESS);

    board.board[GRID(4, 4)] = 'q'; // queen e4
    assert(checkMove(&board, 4, 4, 1, 1) == MOVE_SUCCESS);
    assert(checkMove(&board, 4, 4, 4, 7) == MOVE_SUCCESS);

    board.board[GRID(4, 4)] = 'k'; // king e4
    assert(checkMove(&board, 4, 4, 3, 5) == MOVE_SUCCESS);
    assert(checkMove(&board, 4, 4, 2, 4) == MOVE_INVALID);
}

static void testBlackPieceMoves()
{
    ChessBoard board = emptyBoard(false);
    board.board[E1] = 'k';

    board.board[GRID(3, 3)] = 'B'; // bishop d5
    assert(checkMove(&board, 3, 3, 5, 5) == MOVE_SUCCESS);

    board.board[GRID(3, 3)] = 'R'; // rook d5
    assert(checkMove(&board, 3, 3, 3, 7) == MOVE_SUCCESS);

    board.board[GRID(3, 3)] = 'Q'; // queen d5
    assert(checkMove(&board, 3, 3, 6, 6) == MOVE_SUCCESS);

    board.board[GRID(3, 3)] = 'K'; // king d5
    assert(checkMove(&board, 3, 3, 4, 4) == MOVE_SUCCESS);
}

static void testCaptures()
{
    ChessBoard board = emptyBoard(true);
    board.board[GRID(4, 4)] = 'q';
    board.board[GRID(2, 2)] = 'P';
    board.board[E1] = 'k';
    board.board[E8] = 'K';

    assert(checkMove(&board, 4, 4, 2, 2) == MOVE_SUCCESS);

    board.board[GRID(2, 2)] = 'q';
    assert(checkMove(&board, 4, 4, 2, 2) == MOVE_INVALID);
}

static void testBlockedSlidingPieces()
{
    ChessBoard board = emptyBoard(true);
    board.board[GRID(4, 4)] = 'q';
    board.board[GRID(3, 3)] = 'p';
    board.board[E1] = 'k';
    board.board[E8] = 'K';

    assert(checkMove(&board, 4, 4, 2, 2) == MOVE_INVALID);
}

static void testCastling()
{
    ChessBoard board = emptyBoard(true);
    board.castlingRights =
        CASTLE_WHITE_KINGSIDE | CASTLE_WHITE_QUEENSIDE;

    board.board[E1] = 'k';
    board.board[H1] = 'r';
    board.board[A1] = 'r';
    board.board[E8] = 'K';

    assert(movePieceChessNotation(&board, "e1g1") == MOVE_SUCCESS);

    board = emptyBoard(true);
    board.castlingRights = CASTLE_WHITE_QUEENSIDE;
    board.board[E1] = 'k';
    board.board[A1] = 'r';
    board.board[E8] = 'K';

    assert(movePieceChessNotation(&board, "e1c1") == MOVE_SUCCESS);
}

static void testEnPassant()
{
    ChessBoard board = emptyBoard(true);
    board.board[E2] = 'p';
    board.board[D4] = 'P';
    board.board[E1] = 'k';
    board.board[E8] = 'K';

    assert(movePieceChessNotation(&board, "e2e4") == MOVE_SUCCESS);

    assert(board.enPassantSquare == E3);
    assert(board.playerTurn == false);

    assert(movePieceChessNotation(&board, "d4e3") == MOVE_SUCCESS);
    assert(board.board[D4] == ' ');
    assert(board.board[E3] == 'P');
    assert(board.board[E4] == ' ');
    assert(board.enPassantSquare == NO_EN_PASSANT);
}

int main()
{
    RUN_TEST(testResetBoard);
    RUN_TEST(testWhitePawnMoves);
    RUN_TEST(testKnightCanJump);
    RUN_TEST(testRookPathBlocked);
    RUN_TEST(testMoveUpdatesBoard);
    RUN_TEST(testInvalidCoordinates);
    RUN_TEST(testWhitePieceMoves);
    RUN_TEST(testBlackPieceMoves);
    RUN_TEST(testCaptures);
    RUN_TEST(testBlockedSlidingPieces);
    RUN_TEST(testCastling);
    RUN_TEST(testEnPassant);
    return 0;
}
