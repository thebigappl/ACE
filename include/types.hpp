#pragma once

#include <array>
#include <random>
using namespace std;
using U64 = unsigned long long;
using Square = int;
using Piece = int;

inline constexpr char NAME[] = "ACE";
inline constexpr char AUTHOR[] = "Aaron Pech";
inline constexpr int NUM_SQUARES = 120;

enum {
    wP, wN, wB, wR, wQ, wK,
    bP, bN, bB, bR, bQ, bK,
    EMPTY, OFFBOARD
};

// Use this enum for general piece type and indexing
enum {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NONE
};

enum {
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE
};

enum {
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE
};

enum {
    WHITE, BLACK, BOTH
};

enum {
    KINGSIDE, QUEENSIDE
};

enum {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ
};

enum {
    EXACT, LOWERBOUND, UPPERBOUND
};

struct Position {
    Piece pieces[NUM_SQUARES];

    array<Square, 2> kingSquares = {E1, E8};

    int sideToMove = WHITE;
    array<bool, 4> canCastle = {true, true, true, true}; // wK, wQ, bK, bQ
    Square enPassantSquare = -1;
    int ply = 0;
    int halfMoveClock = 0;

    U64 hash = 0;

    // Initialise a board containing 64 EMPTY squares and the remaining 56 are OFFBOARD
    Position() {
        for (int i = 0; i < NUM_SQUARES; i++) {
            pieces[i] = OFFBOARD;
        }

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                pieces[(i + 2) * 10 + (j + 1)] = EMPTY;
            }
        }
    }
};

struct Move {
    Square from;
    Square to;
    Piece promotionPiece = EMPTY;

    bool operator==(Move other) {
        return from == other.from &&
                to == other.to &&
                promotionPiece == other.promotionPiece;
    }
};

struct UndoInfo {
    Piece movedPiece;
    Piece capturedPiece;
    Square captureSquare;

    array<Square, 2> previousKingSquares;

    Square previousEnPassantSquare;
    array<bool, 4> previousCanCastle;

    int previousHalfMoveClock;
    U64 previousHash;
};

struct SearchLimits {
    int depth = 0;

    int wtime = -1;
    int btime = -1;

    int winc = 0;
    int binc = 0;

    int movestogo = 0;
};

struct SearchResult {
    Move bestMove{};
    int score = 0;
    U64 nodes = 0;
};

struct MoveList {
    Move moves[256];
    int count = 0;

    Move* begin() {
        return moves;
    }
    Move* end() {
        return moves + count;
    }
    const Move* begin() const {
        return moves;
    }
    const Move* end() const {
        return moves + count;
    }
};

struct ScoredMove {
    Move move;
    int priorityScore = 1;
};

struct ZobristKey {
    // Each piece-square combination
    U64 pieceSquareKey[12][64];

    U64 sideToMoveKey;

    // Each combination of castling rights
    U64 castlingKey[16];

    // Each en passant file
    U64 enPassantKey[8];

    ZobristKey() {
        mt19937_64 generator(12345);
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < 64; j++) {
                pieceSquareKey[i][j] = generator();
            }
        }

        sideToMoveKey = generator();

        for (int i = 0; i < 16; i++) {
            castlingKey[i] = generator();
        }

        for (int i = 0; i < 8; i++) {
            enPassantKey[i] = generator();
        }
    }
};

struct TTEntry {
    U64 hash;
    int depth = -1;
    int score;
    int boundType;
    Move bestMove;
    bool occupied;
};
