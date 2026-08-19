#include <cstdio>

extern "C" {
#include "ChessBoard.h"
}

static int failures = 0;

#define CHECK(condition)                                      \
    do {                                                      \
        if (!(condition)) {                                   \
            std::printf("[FAIL] %s:%d: %s\n",                \
                        __FILE__, __LINE__, #condition);      \
            ++failures;                                       \
        }                                                     \
    } while (false)

#define RUN_TEST(test) \
    do { \
        int prefail = failures; \
        test(); \
        if (prefail == failures) \
            std::printf("[PASS] %s\n", #test); \
        else \
            std::printf("[FAIL] %s\n", #test); \
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

    CHECK(board.board[A1] == 'r');
    CHECK(board.board[E1] == 'k');
    CHECK(board.board[A8] == 'R');
    CHECK(board.board[E8] == 'K');
    CHECK(board.playerTurn);
    CHECK(board.castlingRights == 0xF);
    CHECK(board.enPassantSquare == NO_EN_PASSANT);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');
}

static void testWhitePawnMoves()
{
    ChessBoard board = emptyBoard(true);
    board.board[GRID(6, 4)] = 'p';
    board.board[E1] = 'k';
    board.board[E8] = 'K';
    board.whiteKingSquare = E1;
    board.blackKingSquare = E8;

    CHECK(checkMove(&board, 6, 4, 5, 4) == MOVE_SUCCESS);
    CHECK(checkMove(&board, 6, 4, 4, 4) == MOVE_SUCCESS);
    CHECK(checkMove(&board, 6, 4, 6, 5) == MOVE_INVALID);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');
}

static void testKnightCanJump()
{
    ChessBoard board = emptyBoard(true);
    board.board[GRID(7, 1)] = 'n';
    board.board[E1] = 'k';
    board.board[E8] = 'K';
    board.whiteKingSquare = E1;
    board.blackKingSquare = E8;

    CHECK(checkMove(&board, 7, 1, 5, 2) == MOVE_SUCCESS);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');
}

static void testRookPathBlocked()
{
    ChessBoard board = emptyBoard(true);
    board.board[GRID(7, 0)] = 'r';
    board.board[GRID(6, 0)] = 'p';
    board.board[E1] = 'k';
    board.board[E8] = 'K';
    board.whiteKingSquare = E1;
    board.blackKingSquare = E8;

    CHECK(checkMove(&board, 7, 0, 5, 0) == MOVE_INVALID);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');
}

static void testMoveUpdatesBoard()
{
    ChessBoard board = emptyBoard(true);
    board.board[E2] = 'p';
    board.board[E1] = 'k';
    board.board[E8] = 'K';
    board.whiteKingSquare = E1;
    board.blackKingSquare = E8;

    CHECK(movePieceChessNotation(&board, "e2e4") == MOVE_SUCCESS);
    CHECK(board.board[E2] == ' ');
    CHECK(board.board[E4] == 'p');
    CHECK(!board.playerTurn);
    CHECK(board.enPassantSquare == E3);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');
}

static void testInvalidCoordinates()
{
    ChessBoard board = emptyBoard(true);
    board.board[E1] = 'k';
    board.board[E8] = 'K';
    board.whiteKingSquare = E1;
    board.blackKingSquare = E8;

    CHECK(checkMove(&board, 8, 0, 7, 0) == MOVE_INVALID);
    CHECK(checkMove(&board, 7, 0, 8, 0) == MOVE_INVALID);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');
}

static void testWhitePieceMoves()
{
    ChessBoard board = emptyBoard(true);
    board.board[E1] = 'k';
    board.whiteKingSquare = E1;
    board.board[E8] = 'K';
    board.blackKingSquare = E8;

    board.board[GRID(4, 4)] = 'b'; // bishop e4
    CHECK(checkMove(&board, 4, 4, 2, 2) == MOVE_SUCCESS);
    CHECK(checkMove(&board, 4, 4, 4, 6) == MOVE_INVALID);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');

    board.board[GRID(4, 4)] = 'r'; // rook e4
    CHECK(checkMove(&board, 4, 4, 4, 7) == MOVE_SUCCESS);
    CHECK(checkMove(&board, 4, 4, 1, 4) == MOVE_SUCCESS);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');

    board.board[GRID(4, 4)] = 'q'; // queen e4
    CHECK(checkMove(&board, 4, 4, 1, 1) == MOVE_SUCCESS);
    CHECK(checkMove(&board, 4, 4, 4, 7) == MOVE_SUCCESS);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');

    board.board[E1] = ' ';
    board.board[GRID(4, 4)] = 'k'; // king e4
    board.whiteKingSquare = E4;
    CHECK(checkMove(&board, 4, 4, 3, 5) == MOVE_SUCCESS);
    CHECK(checkMove(&board, 4, 4, 2, 4) == MOVE_INVALID);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');
}

static void testBlackPieceMoves()
{
    ChessBoard board = emptyBoard(false);
    board.board[E1] = 'k';
    board.whiteKingSquare = E1;
    board.board[E8] = 'K';
    board.blackKingSquare = E8;

    board.board[GRID(3, 3)] = 'B'; // bishop d5
    CHECK(checkMove(&board, 3, 3, 5, 5) == MOVE_SUCCESS);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');

    board.board[GRID(3, 3)] = 'R'; // rook d5
    CHECK(checkMove(&board, 3, 3, 3, 7) == MOVE_SUCCESS);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');

    board.board[GRID(3, 3)] = 'Q'; // queen d5
    CHECK(checkMove(&board, 3, 3, 6, 6) == MOVE_SUCCESS);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');

    board.board[E8] = ' ';
    board.board[GRID(3, 3)] = 'K'; // king d5
    board.blackKingSquare = D5;
    CHECK(checkMove(&board, 3, 3, 4, 4) == MOVE_SUCCESS);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');
}

static void testCaptures()
{
    ChessBoard board = emptyBoard(true);
    board.board[GRID(4, 4)] = 'q';
    board.board[GRID(2, 2)] = 'P';
    board.board[E1] = 'k';
    board.board[E8] = 'K';
    board.whiteKingSquare = E1;
    board.blackKingSquare = E8;

    CHECK(checkMove(&board, 4, 4, 2, 2) == MOVE_SUCCESS);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');

    board.board[GRID(2, 2)] = 'q';
    CHECK(checkMove(&board, 4, 4, 2, 2) == MOVE_INVALID);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');
}

static void testBlockedSlidingPieces()
{
    ChessBoard board = emptyBoard(true);
    board.board[GRID(4, 4)] = 'q';
    board.board[GRID(3, 3)] = 'p';
    board.board[E1] = 'k';
    board.board[E8] = 'K';
    board.whiteKingSquare = E1;
    board.blackKingSquare = E8;

    CHECK(checkMove(&board, 4, 4, 2, 2) == MOVE_INVALID);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');
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
    board.whiteKingSquare = E1;
    board.blackKingSquare = E8;

    CHECK(movePieceChessNotation(&board, "e1g1") == MOVE_SUCCESS);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');

    board = emptyBoard(true);
    board.castlingRights = CASTLE_WHITE_QUEENSIDE;
    board.board[E1] = 'k';
    board.board[A1] = 'r';
    board.board[E8] = 'K';
    board.whiteKingSquare = E1;
    board.blackKingSquare = E8;

    CHECK(movePieceChessNotation(&board, "e1c1") == MOVE_SUCCESS);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');
}

static void testEnPassant()
{
    ChessBoard board = emptyBoard(true);
    board.board[E2] = 'p';
    board.board[D4] = 'P';
    board.board[E1] = 'k';
    board.board[E8] = 'K';
    board.whiteKingSquare = E1;
    board.blackKingSquare = E8;

    CHECK(movePieceChessNotation(&board, "e2e4") == MOVE_SUCCESS);
    CHECK(board.enPassantSquare == E3);
    CHECK(board.playerTurn == false);
    CHECK(board.board[board.whiteKingSquare] == 'k');
    CHECK(board.board[board.blackKingSquare] == 'K');

    CHECK(movePieceChessNotation(&board, "d4e3") == MOVE_SUCCESS);
    CHECK(board.board[D4] == ' ');
    CHECK(board.board[E3] == 'P');
    CHECK(board.board[E4] == ' ');
    CHECK(board.enPassantSquare == NO_EN_PASSANT);
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

    std::printf("%d failure(s)\n", failures);
    return failures == 0 ? 0 : 1;
}
