#include "types.hpp"

// Gets the general pieceType (PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING or NONE)
int getPieceType(Piece piece) {
    if (piece == EMPTY || piece == OFFBOARD) return NONE;
    return piece % 6;
}

// Checks if the targetPiece is of the same colour as the side to move
bool isSameColour(int colour, Piece targetPiece) {
    if (colour == WHITE && (targetPiece >= wP && targetPiece <= wK)) return true;
    if (colour == BLACK && (targetPiece >= bP && targetPiece <= bK)) return true;
    return false;
}
