#include "types.hpp"
#include "movegen.hpp"
#include "pieces.hpp"
#include "constants.hpp"
#include "board.hpp"
#include "attacks.hpp"
#include <vector>
#include <array>
using namespace std;

// This function assumes that `move` is a king move
bool isCastle(Move move) {
    int dist = move.to - move.from;
    if (dist == 2 || dist == -2) return true;
    return false;
}

UndoInfo makeMove(Position& pos, Move move) {
    Piece *board = pos.pieces;

    // Store previous state
    UndoInfo undo;
    undo.movedPiece = board[move.from];
    undo.capturedPiece = board[move.to];
    undo.captureSquare = move.to;
    undo.previousKingSquares = pos.kingSquares;
    undo.previousCanCastle = pos.canCastle;
    undo.previousEnPassantSquare = pos.enPassantSquare;
    undo.previousHalfMoveClock = pos.halfMoveClock;
    undo.previousHash = pos.hash;

    // XOR out the original castling rights from the hash
    int castlingIndex = 0;
    for (int i = 0, j = 8; i < 4; i++, j /= 2) {
        if (pos.canCastle[i]) castlingIndex += j;
    }
    pos.hash ^= zobrist.castlingKey[castlingIndex];

    // XOR out the original en passant square from the hash if it existed
    if (pos.enPassantSquare != -1) pos.hash ^= zobrist.enPassantKey[squareFile[pos.enPassantSquare]];

    bool diagonal = isDiagonalDirection(move.to - move.from);
    int m64From = getM64Index(move.from);
    int m64To = getM64Index(move.to);

    // Modify current position state
    Piece movingPieceType = pieceType[board[move.from]];
    Piece movingPiece = board[move.from];
    Piece targetPiece = board[move.to];
    board[move.to] = board[move.from];
    board[move.from] = EMPTY;

    // XOR out the original piece positions before the move
    pos.hash ^= zobrist.pieceSquareKey[movingPiece][m64From];
    if (targetPiece != EMPTY) pos.hash ^= zobrist.pieceSquareKey[targetPiece][m64To];

    // Check if the king moved
    if (movingPieceType == KING) {
        pos.kingSquares[pos.sideToMove] = move.to;
        // Castling
        if (isCastle(move)) {
            Square rookSquare;
            Square newRookSquare;
            // Castling kingside
            if (move.to > move.from) {
                rookSquare = move.from + 3;
                newRookSquare = move.to - 1;
            } else {
                rookSquare = move.from - 4;
                newRookSquare = move.to + 1;
            }
            Piece rook = board[rookSquare];
            board[newRookSquare] = board[rookSquare];
            board[rookSquare] = EMPTY;
            // XOR out old rook position
            pos.hash ^= zobrist.pieceSquareKey[rook][getM64Index(rookSquare)];
            // XOR in new rook position
            pos.hash ^= zobrist.pieceSquareKey[rook][getM64Index(newRookSquare)];
        }
        // Remove colour's right to castle on both sides
        for (int i = 0; i < 2; i++) {
            pos.canCastle[pos.sideToMove * 2 + i] = false;
        }
    }

    // Check if a rook moved
    if (movingPieceType == ROOK) {
        for (int i = 0; i < 4; i++) {
            if (move.from == originalRookPositions[i]) pos.canCastle[i] = false;
        }
    }

    // Check if a rook was captured
    if (pieceType[undo.capturedPiece] == ROOK) {
        for (int i = 0; i < 4; i++) {
            if (move.to == originalRookPositions[i]) pos.canCastle[i] = false;
        }
    }

    // Check if move is irreversible (by the rules of the game)
    if (movingPieceType == PAWN || undo.capturedPiece != EMPTY) pos.halfMoveClock = 0;
    else pos.halfMoveClock++;

    // Check promotion
    if (movingPieceType == PAWN &&
        isPromotionRank(pos.sideToMove, squareRank[move.to])) {
        board[move.to] = move.promotionPiece;
        // XOR in newly promoted piece
        pos.hash ^= zobrist.pieceSquareKey[move.promotionPiece][m64To];
    } else {
        // XOR in moving piece
        pos.hash ^= zobrist.pieceSquareKey[movingPiece][m64To];
    }

    // Check en passant
    if (movingPieceType == PAWN &&
        move.to == pos.enPassantSquare &&
        diagonal) {
        int captureSquare = move.to - pawnDirections[pos.sideToMove] * 10;
        int capturePiece = board[captureSquare];
        undo.capturedPiece = capturePiece;
        undo.captureSquare = captureSquare;
        // XOR out captured pawn en passant
        pos.hash ^= zobrist.pieceSquareKey[capturePiece][getM64Index(captureSquare)];
        board[captureSquare] = EMPTY;
    }

    // Check double pawn push
    if (movingPieceType == PAWN &&
        (move.to - move.from) * pawnDirections[pos.sideToMove] == 20) {
        pos.enPassantSquare = (move.to + move.from) / 2;
        // XOR in new en passant square
        pos.hash ^= zobrist.enPassantKey[squareFile[pos.enPassantSquare]];
    } else {
        pos.enPassantSquare = -1;
    }

    // XOR in new castling state
    castlingIndex = 0;
    for (int i = 0, j = 8; i < 4; i++, j /= 2) {
        if (pos.canCastle[i]) castlingIndex += j;
    }
    pos.hash ^= zobrist.castlingKey[castlingIndex];

    pos.sideToMove ^= 1;
    pos.hash ^= zobrist.sideToMoveKey;
    pos.hashHist[pos.ply++] = pos.hash;

    return undo;
}

void unmakeMove(Position& pos, Move move, const UndoInfo& undo) {
    Piece *board = pos.pieces;

    // Move the moving piece back to its original square
    board[move.from] = undo.movedPiece;
    board[move.to] = EMPTY;

    // Place the captured piece back on the capture square
    board[undo.captureSquare] = undo.capturedPiece;

    // If last move was a castle, also move the rook back
    if (isCastle(move) && pieceType[undo.movedPiece] == KING) {
        Square rookSquare, origRookSquare;
        if (move.to > move.from) {
            rookSquare = move.to - 1;
            origRookSquare = move.to + 1;
        } else {
            rookSquare = move.to + 1;
            origRookSquare = move.to - 2;
        }
        board[origRookSquare] = board[rookSquare];
        board[rookSquare] = EMPTY;
    }

    // Simple reverts
    pos.kingSquares = undo.previousKingSquares;
    pos.canCastle = undo.previousCanCastle;
    pos.enPassantSquare = undo.previousEnPassantSquare;
    pos.halfMoveClock = undo.previousHalfMoveClock;
    pos.sideToMove ^= 1;
    pos.ply--;
    pos.hash = undo.previousHash;
}