#include "types.hpp"
#include "board.hpp"
#include "constants.hpp"
#include "pieces.hpp"

void initialisePositionHash(Position& pos) {
    U64 localHash = 0;
    for (int i = 0; i < NUM_SQUARES; i++) {
        Piece targetPiece = pos.pieces[i];
        if (targetPiece == OFFBOARD || targetPiece == EMPTY) continue;

        int mb64Square = m64Square[i];
        localHash ^= RANDOM64[64 * polyglotPieceIndex[targetPiece] + mb64Square];
    }

    if (pos.sideToMove == WHITE) {
        localHash ^= RANDOM64[SIDE_TO_MOVE_OFFSET];
    }

    for (int i = 0; i < 4; i++) {
        if (pos.canCastle[i]) {
            localHash ^= RANDOM64[CASTLING_OFFSET + i];
        }
    }

    if (pos.enPassantSquare != -1) {
        int left = pos.enPassantSquare - pawnDirections[pos.sideToMove] * 9;
        int right = pos.enPassantSquare - pawnDirections[pos.sideToMove] * 11;
        if ((pieceType[pos.pieces[left]] == PAWN &&
            isSameColour(pos.sideToMove, pos.pieces[left])) ||
            (pieceType[pos.pieces[right]] == PAWN &&
            isSameColour(pos.sideToMove, pos.pieces[right]))) {
            localHash ^= RANDOM64[EN_PASSANT_OFFSET + squareFile[pos.enPassantSquare]];
        }
    }
    pos.hash = localHash;
}

// Assumes that `pos` is an empty board
// This function is not well designed lol
void initialiseStartingPosition(Position& pos) {
    // Reset board back to being empty
    for (int i = 0; i < NUM_SQUARES; i++) {
            pos.pieces[i] = OFFBOARD;
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            pos.pieces[(i + 2) * 10 + (j + 1)] = EMPTY;
        }
    }
    
    // Reset all other information about the current position
    pos.kingSquares[WHITE] = E1;
    pos.kingSquares[BLACK] = E8;
    pos.sideToMove = WHITE;
    pos.canCastle.fill(true);
    pos.enPassantSquare = -1;
    pos.ply = 0;
    pos.halfMoveClock = 0;

    // Place pieces on their starting squares
    // (There's probably a more efficient way to do this but its fine for now...)
    Piece *board = pos.pieces;

    // White
    board[A1] = wR;
    board[B1] = wN;
    board[C1] = wB;
    board[D1] = wQ;
    board[E1] = wK;
    board[F1] = wB;
    board[G1] = wN;
    board[H1] = wR;
    for (int i = A2; i <= H2; i++) {
        board[i] = wP;
    }

    // Black
    board[A8] = bR;
    board[B8] = bN;
    board[C8] = bB;
    board[D8] = bQ;
    board[E8] = bK;
    board[F8] = bB;
    board[G8] = bN;
    board[H8] = bR;
    for (int i = A7; i <= H7; i++) {
        board[i] = bP;
    }

    initialisePositionHash(pos);
}
