#include "types.hpp"
#include "constants.hpp"
#include "board.hpp"
#include "attacks.hpp"
#include "pieces.hpp"
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
    for (int i = 0; i < 4; i++) {
        if (pos.canCastle[i]) {
            pos.hash ^= RANDOM64[CASTLING_OFFSET + i];
        }
    }

    // XOR out the original en passant square from the hash if it existed
    if (pos.enPassantSquare != -1) {
        int left = pos.enPassantSquare - pawnDirections[pos.sideToMove] * 9;
        int right = pos.enPassantSquare - pawnDirections[pos.sideToMove] * 11;
        if ((pieceType[pos.pieces[left]] == PAWN &&
            isSameColour(pos.sideToMove, pos.pieces[left])) ||
            (pieceType[pos.pieces[right]] == PAWN &&
            isSameColour(pos.sideToMove, pos.pieces[right]))) {
            pos.hash ^= RANDOM64[EN_PASSANT_OFFSET + squareFile[pos.enPassantSquare]];
        }
    }

    bool diagonal = isDiagonalDirection(move.to - move.from);
    int m64From = m64Square[move.from];
    int m64To = m64Square[move.to];

    // Modify current position state
    Piece movingPieceType = pieceType[board[move.from]];
    Piece movingPiece = board[move.from];
    Piece targetPiece = board[move.to];
    board[move.to] = board[move.from];
    board[move.from] = EMPTY;

    // XOR out the original piece positions before the move
    pos.hash ^= RANDOM64[64 * polyglotPieceIndex[movingPiece] + m64From];
    if (targetPiece != EMPTY) pos.hash ^= RANDOM64[64 * polyglotPieceIndex[targetPiece] + m64To];

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
            pos.hash ^= RANDOM64[64 * polyglotPieceIndex[rook] + m64Square[rookSquare]];
            // XOR in new rook position
            pos.hash ^= RANDOM64[64 * polyglotPieceIndex[rook] + m64Square[newRookSquare]];
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
        pos.hash ^= RANDOM64[64 * polyglotPieceIndex[move.promotionPiece] + m64To];
    } else {
        // XOR in moving piece
        pos.hash ^= RANDOM64[64 * polyglotPieceIndex[movingPiece] + m64To];
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
        pos.hash ^= RANDOM64[64 * polyglotPieceIndex[capturePiece] + m64Square[captureSquare]];
        board[captureSquare] = EMPTY;
    }

    // Check double pawn push
    if (movingPieceType == PAWN &&
        (move.to - move.from) * pawnDirections[pos.sideToMove] == 20) {
        pos.enPassantSquare = (move.to + move.from) / 2;
        // XOR in new en passant square
        int nextSide = pos.sideToMove ^ 1;
        int left = pos.enPassantSquare - pawnDirections[nextSide] * 9;
        int right = pos.enPassantSquare - pawnDirections[nextSide] * 11;
        if ((pieceType[pos.pieces[left]] == PAWN &&
            isSameColour(nextSide, pos.pieces[left])) ||
            (pieceType[pos.pieces[right]] == PAWN &&
            isSameColour(nextSide, pos.pieces[right]))) {
            pos.hash ^= RANDOM64[EN_PASSANT_OFFSET + squareFile[pos.enPassantSquare]];
        }
    } else {
        pos.enPassantSquare = -1;
    }

    // XOR in new castling state
    for (int i = 0; i < 4; i++) {
        if (pos.canCastle[i]) {
            pos.hash ^= RANDOM64[CASTLING_OFFSET + i];
        }
    }

    pos.sideToMove ^= 1;
    pos.hash ^= RANDOM64[SIDE_TO_MOVE_OFFSET];
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