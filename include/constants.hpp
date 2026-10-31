#pragma once

#include "types.hpp"

// movegen
inline constexpr bool isSlidingPiece[6] = {false, false, true, true, true, false};
inline constexpr int numDirections[6] = {0, 8, 4, 4, 8, 8};
inline constexpr int moveDirections[6][8] = {
    {   0,   0,  0,  0, 0,  0,  0,  0 },
    { -21, -19,-12, -8, 8, 12, 19, 21 }, // Knight
    { -11,  -9,  9, 11, 0,  0,  0,  0 }, // Bishop
    { -10,  -1,  1, 10, 0,  0,  0,  0 }, // Rook
    { -11, -10, -9, -1, 1,  9, 10, 11 }, // Queen
    { -11, -10, -9, -1, 1,  9, 10, 11 }  // King
};
inline constexpr int pawnDirections[2] = {1, -1};
inline constexpr int captureDirections[2] = {11, 9};
inline constexpr int castlingDirections[2] = {1, -1};
inline constexpr int originalRookPositions[4] = {H1, A1, H8, A8};

// eval
inline constexpr int pieceValues[6] = {100, 300, 300, 500, 900, 20000};
inline constexpr int PROMO_MULT = 100;
inline constexpr int CAPTURE_MULT = 10;
inline constexpr int CAPTURE_BASE_SCORE = 10000;

// search
inline constexpr int MATE_SCORE = 100000;
inline constexpr int INFINITY_SCORE = 1000000;
inline constexpr int MAX_Q_DEPTH = 6;
inline constexpr int MAX_PLY = 16;

// transposition tables
inline ZobristKey zobrist;
inline TTEntry transpositionTable[1048576];
inline constexpr int indexMask = (1 << 20) - 1;

// other
inline constexpr int squareRank[120] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  1,  1,  1,  1,  1,  1,  1,  1, -1,
    -1,  2,  2,  2,  2,  2,  2,  2,  2, -1,
    -1,  3,  3,  3,  3,  3,  3,  3,  3, -1,
    -1,  4,  4,  4,  4,  4,  4,  4,  4, -1,
    -1,  5,  5,  5,  5,  5,  5,  5,  5, -1,
    -1,  6,  6,  6,  6,  6,  6,  6,  6, -1,
    -1,  7,  7,  7,  7,  7,  7,  7,  7, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

inline constexpr int squareFile[120] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

inline constexpr int pieceType[14] = {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
    NONE, NONE
};