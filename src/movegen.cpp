#include "types.hpp"
#include "movegen.hpp"
#include "pieces.hpp"
#include "constants.hpp"
#include "board.hpp"
#include "attacks.hpp"
#include "move.hpp"
#include <vector>
#include <array>
using namespace std;

// This helper function assumes that `targetSquare` lies on the promotion rank
void addPawnPromotionMoves(Piece pawn, int from, int targetSquare, MoveList& moveList) {
    for (int i = 1; i <= 4; i++) {
        moveList.moves[moveList.count++] = {from, targetSquare, pawn + i};
    }
}

// Assumes `from` contains a sliding piece of the side to move.
void addSlidingMoves(const Position& pos, int from, int pieceType, MoveList& moveList) {
    // Sliding move generation only
    const Piece *board = pos.pieces;
    int colour = pos.sideToMove;
    for (int direction : moveDirections[pieceType]) {
        for (int i = 1; i < 8; i++) {
            int targetSquare = from + i * direction;
            int targetPiece = board[targetSquare];

            if (targetPiece == OFFBOARD) break;

            if (targetPiece == EMPTY) {
                moveList.moves[moveList.count++] = {from, targetSquare};
                continue;
            }
            if (isSameColour(colour, targetPiece)) {
                break;
            } else {
                moveList.moves[moveList.count++] = {from, targetSquare};
                break;
            }
        }
    }
}

void addKnightMoves(const Position& pos, int from, MoveList& moveList) {
    const Piece *board = pos.pieces;
    int colour = pos.sideToMove;
    for (int direction : moveDirections[KNIGHT]) {
        int targetSquare = from + direction;
        int targetPiece = board[targetSquare];

        if (targetPiece == OFFBOARD) continue;
        if (targetPiece == EMPTY || !isSameColour(colour, targetPiece)) {
            moveList.moves[moveList.count++] = {from, targetSquare};
        }
    }
}

void addPawnMoves(const Position& pos, int from, MoveList& moveList) {
    const Piece *board = pos.pieces;
    int colour = pos.sideToMove;
    int enPassantSquare = pos.enPassantSquare;
    int fromRank = squareRank[from];
    int pawnDirection = pawnDirections[colour];
    int forwardDirection = pawnDirection * 10;

    // First check normal pawn movement
    int targetSquare = from + forwardDirection;
    int targetRank = squareRank[targetSquare];
    int targetPiece = board[targetSquare];
    if (targetPiece == EMPTY) {
        // To promote or not
        if (isPromotionRank(colour, targetRank)) {
            addPawnPromotionMoves(board[from], from, targetSquare, moveList);
        } else {
            moveList.moves[moveList.count++] = {from, targetSquare};
        }

        // Pawn double push
        if (isStartingPosition(colour, fromRank)) {
            int doubleTarget = targetSquare + forwardDirection;

            if (board[doubleTarget] == EMPTY) {
                moveList.moves[moveList.count++] = {from, doubleTarget};
            }
        }
    }

    // Check pawn captures (including en passant)
    for (int direction : captureDirections) {
        targetSquare = from + direction * pawnDirection;
        targetPiece = board[targetSquare];
        if ((targetPiece != EMPTY && targetPiece != OFFBOARD && !isSameColour(colour, targetPiece)) ||
            (targetSquare == enPassantSquare && enPassantSquare != -1)) {
            if (isPromotionRank(colour, targetRank)) {
                addPawnPromotionMoves(board[from], from, targetSquare, moveList);
            } else {
                moveList.moves[moveList.count++] = {from, targetSquare};
            }
        }
    }
}

void addKingMoves(const Position& pos, int from, MoveList& moveList) {
    const Piece *board = pos.pieces;
    int colour = pos.sideToMove;
    for (int direction : moveDirections[KING]) {
        int targetSquare = from + direction;
        int targetPiece = board[targetSquare];

        if (targetPiece == OFFBOARD) continue;
        if (targetPiece == EMPTY || !isSameColour(colour, targetPiece)) {
            moveList.moves[moveList.count++] = {from, targetSquare};
        }
    }

    for (int i = 0; i < 2; i++) {
        if (pos.canCastle[colour * 2 + i]) {
            int numSquares = (i == KINGSIDE) ? 2 : 3;
            bool valid = true;
            for (int j = 1; j <= numSquares; j++) {
                int targetSquare = from + j * castlingDirections[i];
                int targetPiece = board[targetSquare];

                if (targetPiece != EMPTY) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                moveList.moves[moveList.count++] = {from, from + castlingDirections[i] * 2};
            }
        }
    }
}


// Focusing on only pseudo-legal moves currently
// This function will only be for generating pseudo-legal moves as well
// A new function `generateLegalMoves` will be used for filtering out illegal pseudo-legal moves
MoveList generateMoves(const Position& pos) {
    MoveList moveList;
    const Piece *board = pos.pieces;
    int colour = pos.sideToMove;

    for (int i = 0; i < NUM_SQUARES; i++) {
        Piece piece = board[i];
        if (piece == OFFBOARD || piece == EMPTY) continue;
        if (!isSameColour(colour, piece)) continue;

        int type = pieceType[piece];
        if (isSlidingPiece[type]) {
            addSlidingMoves(pos, i, type, moveList);
        } else if (type == KNIGHT) {
            addKnightMoves(pos, i, moveList);
        } else if (type == PAWN) {
            addPawnMoves(pos, i, moveList);
        } else if (type == KING) {
            addKingMoves(pos, i, moveList);
        }
        // etc...
    }

    return moveList;
}

MoveList generateLegalMoves(const Position& pos) {
    Position testPos = pos;

    MoveList pseudoLegalMoveList = generateMoves(pos);
    MoveList legalMoves;

    int colour = pos.sideToMove;

    for (const Move& move : pseudoLegalMoveList) {
        bool valid = isCastleLegal(testPos, move);
        UndoInfo undo = makeMove(testPos, move);

        if (isInCheck(testPos, colour)) valid = false;

        unmakeMove(testPos, move, undo);

        if (valid) legalMoves.moves[legalMoves.count++] = move;
    }

    return legalMoves;
}