#include "types.hpp"
#include "pieces.hpp"
#include "constants.hpp"

// This function evaluates who is winning between black and white given the current board position
// It considers material advantages and piece square tables
// Answers from white's perspective
int evaluatePosition(const Position& pos) {
    int materialAdvantage = 0;
    int pieceSquareAdvantage = 0;
    const Piece *board = pos.pieces;
    for (int i = 0; i < NUM_SQUARES; i++) {
        Piece targetPiece = board[i];
        if (targetPiece == OFFBOARD || targetPiece == EMPTY) continue;

        bool isWhite = isSameColour(WHITE, targetPiece);
        int pstSquare = isWhite ? i : i + 10 * (7 - 2 * squareRank[i]);

        int type = pieceType[targetPiece];
        int pieceValue = pieceValues[type];
        int pieceSquareValue = PST[type][pstSquare];

        if (isWhite) {
            materialAdvantage += pieceValue;
            pieceSquareAdvantage += pieceSquareValue;
        } else {
            materialAdvantage -= pieceValue;
            pieceSquareAdvantage -= pieceSquareValue;
        }
    }
    
    return materialAdvantage + pieceSquareAdvantage;
}
