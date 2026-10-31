#include "pieces.hpp"
#include "constants.hpp"
#include "movegen.hpp"
#include "move.hpp"
#include <vector>

// Helper for checking if a move direction is diagonal
bool isDiagonalDirection(int direction) {
    return (direction == -9 || direction == 9 ||
            direction == -11 || direction == 11);
}

//
// Functions for checking basic position validation (king safety through isSquareAttacked)
//
bool isPawnAttack(const Position& pos, int square, int attackingColour) {
    int attackDirection = pawnDirections[attackingColour];
    const Piece *board = pos.pieces;
    for (int direction : captureDirections) {
        int targetSquare = square - attackDirection * direction;
        int targetPiece = board[targetSquare];
        if (targetPiece == OFFBOARD || targetPiece == EMPTY) continue;
        if (pieceType[targetPiece] == PAWN && 
            isSameColour(attackingColour, targetPiece)) return true;
    }
    return false;
}

bool isKnightAttack(const Position& pos, int square, int attackingColour) {
    const Piece *board = pos.pieces;
    for (int direction : moveDirections[KNIGHT]) {
        int targetSquare = square + direction;
        int targetPiece = board[targetSquare];
        if (targetPiece == OFFBOARD || targetPiece == EMPTY) continue;
        if (pieceType[targetPiece] == KNIGHT &&
            isSameColour(attackingColour, targetPiece)) return true;
    }
    return false;
}

bool isSlidingAttack(const Position& pos, int square, int attackingColour) {
    const Piece *board = pos.pieces;
    for (int direction : moveDirections[QUEEN]) {
        for (int i = 1; i < 8; i++) {
            int targetSquare = square + i * direction;
            int targetPiece = board[targetSquare];

            if (targetPiece == OFFBOARD) break;

            if (targetPiece == EMPTY) continue;

            if (!isSameColour(attackingColour, targetPiece)) break;

            int type = pieceType[targetPiece];
            bool diagonal = isDiagonalDirection(direction);

            if (diagonal) {
                if (type == BISHOP || type == QUEEN) return true;
            } else {
                if (type == ROOK || type == QUEEN) return true;
            }
            break;
        }
    }
    return false;
}

bool isKingAttack(const Position& pos, int square, int attackingColour) {
    const Piece *board = pos.pieces;
    for (int direction : moveDirections[KING]) {
        int targetSquare = square + direction;
        int targetPiece = board[targetSquare];
        if (targetPiece == OFFBOARD || targetPiece == EMPTY) continue;
        if (pieceType[targetPiece] == KING &&
            isSameColour(attackingColour, targetPiece)) return true;
    }
    return false;
}

bool isSquareAttacked(const Position& pos, int square, int attackingColour) {
    if (isPawnAttack(pos, square, attackingColour)) return true;
    if (isKnightAttack(pos, square, attackingColour)) return true;
    if (isKingAttack(pos, square, attackingColour)) return true;
    if (isSlidingAttack(pos, square, attackingColour)) return true;
    return false;
}

bool isInCheck(const Position& pos, int colour) {
    return isSquareAttacked(pos, pos.kingSquares[colour], (colour + 1) % 2);
}

bool isCheckmate(const Position& pos) {
    return isInCheck(pos, pos.sideToMove) && generateLegalMoves(pos).count == 0;
}

bool isStalemate(const Position& pos) {
    return !isInCheck(pos, pos.sideToMove) && generateLegalMoves(pos).count == 0;
}

bool isFiftyMoveDraw(const Position& pos) {
    return pos.halfMoveClock >= 100;
}

bool isCapture(const Position& pos, const Move& move) {
    return pos.pieces[move.to] != EMPTY;
}

bool isCastleLegal(Position& pos, const Move& move) {
    bool valid = true;
    int colour = pos.sideToMove;
    if (pieceType[pos.pieces[move.from]] == KING && isCastle(move)) {
        if (isInCheck(pos, colour)) valid = false;

        Square middleSquare = (move.from + move.to) / 2;
        UndoInfo undo = makeMove(pos, {move.from, middleSquare});

        if (isInCheck(pos, colour)) valid = false;

        unmakeMove(pos, {move.from, middleSquare}, undo);
    }
    return valid;
}

bool isMoveLegal(Position& pos, const Move& move) {
    bool valid = isCastleLegal(pos, move);
    int colour = pos.sideToMove;
    UndoInfo undo = makeMove(pos, move);

    if (isInCheck(pos, colour)) valid = false;

    unmakeMove(pos, move, undo);
    return valid;
}
