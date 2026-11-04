#include "types.hpp"
#include "attacks.hpp"
#include "constants.hpp"
#include "evaluation.hpp"
#include "movegen.hpp"
#include "move.hpp"
#include "pieces.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

int getTTIndex(const Position& pos) {
    return pos.hash & indexMask;
}

void clearTT() {
    fill(transpositionTable, transpositionTable + TT_SIZE, TTEntry{});
}

int movePriorityScore(const Position& pos, const Move& move) {
    int score = 1;
    bool quiet = true;
    // Move is a promotion
    if (move.promotionPiece != EMPTY) {
        quiet = false;
        score += PROMO_MULT *
            (pieceValues[pieceType[move.promotionPiece]] -
            pieceValues[PAWN]);
    }
    // Move is en passant
    if ((pos.pieces[move.from] == wP || pos.pieces[move.from] == bP) &&
        move.to == pos.enPassantSquare) {
        quiet = false;
        score += CAPTURE_BASE_SCORE + (CAPTURE_MULT - 1) * pieceValues[PAWN];
    }
    // Move is normal capture
    if (isCapture(pos, move)) {
        quiet = false;
        score += CAPTURE_BASE_SCORE +
            CAPTURE_MULT * pieceValues[pieceType[move.to]] -
            pieceValues[pieceType[move.from]];
    }
    // Move is quiet
    if (quiet) {
        score = history[move.from][move.to];
    }
    return score;
}

bool priorityComparisonRule(const ScoredMove& a, const ScoredMove& b) {
    return a.priorityScore > b.priorityScore;
}

bool isQuiet(const Position& pos, const Move& move) {
    if (isCapture(pos, move)) return false;
    if (move.promotionPiece != EMPTY) return false;
    if ((pos.pieces[move.from] == wP || pos.pieces[move.from] == bP) &&
        move.to == pos.enPassantSquare) return false;
    return true;
}

// This function does not detect stalemates
int quiescenceSearch(Position& pos, int ply, U64& nodes, int alpha, int beta, int plyExplored) {
    nodes++;
    int standPatScore = evaluatePosition(pos);
    int colour = pos.sideToMove;
    if (colour == BLACK) standPatScore = -standPatScore;
    bool inCheck = isInCheck(pos, colour);

    // Prevent Q-search from exploring too deep into tactical sequences
    if (!inCheck && plyExplored >= MAX_Q_DEPTH) return standPatScore;
    if (!inCheck && standPatScore >= beta) return standPatScore;
    if (!inCheck && standPatScore > alpha) alpha = standPatScore;

    // generate moves only once
    MoveList moveList = generateMoves(pos);

    // Move ordering
    // Create a priority list and sort the moves based on how good they might be
    ScoredMove priorityMoveList[256];
    int n = moveList.count;
    for (int i = 0; i < n; i++) {
        priorityMoveList[i].move = moveList.moves[i];
        priorityMoveList[i].priorityScore = movePriorityScore(pos, priorityMoveList[i].move);
    }
    sort(priorityMoveList, priorityMoveList + n, priorityComparisonRule);

    int bestScore = inCheck ? -INFINITY_SCORE : standPatScore;
    bool isLegal = false;
    for (int i = 0; i < n; i++) {
        const Move& move = priorityMoveList[i].move;
        // Skip quiet moves
        // Skip if the position isn't a check
        bool quiet = isQuiet(pos, move);
        if (!inCheck && quiet) {
            continue;
        }
        
        UndoInfo undo = makeMove(pos, move);
        // Negate the result because the next call to search
        // is from the perspective of the other player
        if (!isInCheck(pos, colour)) {
            int score = -quiescenceSearch(pos, ply + 1, nodes, -beta, -alpha, plyExplored + 1);
            bestScore = max(bestScore, score);
            alpha = max(alpha, score);
            isLegal = true;
        }

        unmakeMove(pos, move, undo);

        if (alpha >= beta) break;
    }
    
    if (!isLegal) return inCheck ? -(MATE_SCORE - ply) : bestScore;
    return bestScore;
}

bool hasLegalMove(Position& pos) {
    MoveList moveList = generateMoves(pos);
    for (const Move& move : moveList) {
        if (isMoveLegal(pos, move)) return true;
    }
    return false;
}

bool isRepetition(const Position& pos) {
    int count = 0;
    int limit = max(0, pos.ply - 1 - pos.halfMoveClock);
    for (int i = pos.ply - 1; i >= limit; i -= 2)
        if (pos.hashHist[i] == pos.hash && ++count >= 2) return true;
    return false;
}

// Recursive search function for finding the best move
int search(Position& pos, int depth, int ply, U64& nodes, int alpha, int beta) {
    nodes++;
    if (isRepetition(pos) || pos.halfMoveClock >= 100) return (ply % 2 == 0) ? -20 : 20;

    int colour = pos.sideToMove;
    bool inCheck = isInCheck(pos, colour);

    if (depth == 0) {
        if (!hasLegalMove(pos)) return inCheck ? -(MATE_SCORE - ply) : 0;
        return quiescenceSearch(pos, ply, nodes, alpha, beta, 0);
    }

    int ttIndex = getTTIndex(pos);
    int originalAlpha = alpha;
    int originalBeta = beta;

    // check old TT entry
    Move hashMove{-1, -1, EMPTY};
    TTEntry entry = transpositionTable[ttIndex];
    if (entry.occupied && entry.hash == pos.hash) {
        hashMove = entry.bestMove;
        int ttScore = entry.score;
        if (abs(abs(ttScore) - MATE_SCORE) <= MAX_PLY) {
            if (ttScore < 0) ttScore += ply;
            else ttScore -= ply;
        }
        if (ply > 2 && entry.depth >= depth && (entry.boundType == EXACT ||
            (entry.boundType == LOWERBOUND && ttScore >= beta) ||
            (entry.boundType == UPPERBOUND && ttScore <= alpha))) {
            return ttScore;
        }
    }
    

    // generate moves only once
    MoveList moveList = generateMoves(pos);

    // Move ordering
    // Create a priority list and sort the moves based on how good they might be
    ScoredMove priorityMoveList[256];
    int n = moveList.count;
    for (int i = 0; i < n; i++) {
        priorityMoveList[i].move = moveList.moves[i];
        if (priorityMoveList[i].move == hashMove) {
            priorityMoveList[i].priorityScore = INFINITY_SCORE;
        } else if (priorityMoveList[i].move == killerMoves[ply][0]) {
            priorityMoveList[i].priorityScore = K_MOVE_1;
        } else if (priorityMoveList[i].move == killerMoves[ply][1]) {
            priorityMoveList[i].priorityScore = K_MOVE_2;
        } else {
            priorityMoveList[i].priorityScore = movePriorityScore(pos, priorityMoveList[i].move);
        }
    }
    sort(priorityMoveList, priorityMoveList + n, priorityComparisonRule);

    int bestScore = -INFINITY_SCORE;
    Move bestMove{-1, -1, EMPTY};
    bool isLegal = false;
    for (int i = 0; i < n; i++) {
        const Move& move = priorityMoveList[i].move;
        if (!isCastleLegal(pos, move)) continue;
        UndoInfo undo = makeMove(pos, move);
        // Negate the result because the next call to search
        // is from the perspective of the other player
        if (!isInCheck(pos, colour)) {
            int score = -search(pos, depth - 1, ply + 1, nodes, -beta, -alpha);
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
            alpha = max(alpha, score);
            isLegal = true;
        }

        unmakeMove(pos, move, undo);

        if (alpha >= beta) {
            // if it was a quiet move:
            if (undo.capturedPiece == EMPTY && move.promotionPiece == EMPTY) {
                if (killerMoves[ply][1] != move) {
                    if (killerMoves[ply][0].from != -1 && killerMoves[ply][0] != move) {
                        killerMoves[ply][1] = killerMoves[ply][0];
                    }
                    killerMoves[ply][0] = move;
                }
                history[move.from][move.to] = min(
                    history[move.from][move.to] + depth * depth -
                    (depth * depth * history[move.from][move.to] / H_MAX),
                    H_MAX);
            }
            break;
        }

        if (undo.capturedPiece == EMPTY && move.promotionPiece == EMPTY) {
            history[move.from][move.to] = max(
                history[move.from][move.to] - depth * depth -
                (depth * depth * history[move.from][move.to] / H_MAX),
                -H_MAX);
        }
    }

    if (!isLegal) return inCheck ? -(MATE_SCORE - ply) : 0;

    int ttScore = bestScore;
    if (abs(abs(bestScore) - MATE_SCORE) <= MAX_PLY) {
        if (bestScore < 0) ttScore -= ply;
        else ttScore += ply;
    }
    entry.bestMove = bestMove;
    entry.depth = depth;
    entry.hash = pos.hash;
    entry.score = ttScore;
    entry.occupied = true;

    if (bestScore <= originalAlpha) {
        entry.boundType = UPPERBOUND;
    } else if (bestScore >= originalBeta) {
        entry.boundType = LOWERBOUND;
    } else {
        entry.boundType = EXACT;
    }

    if (depth >= transpositionTable[ttIndex].depth) {
        transpositionTable[ttIndex] = entry;
    }

    return bestScore;
}

SearchResult findBestMove(Position& pos, int depth) {
    // clearTT();
    MoveList moveList = generateMoves(pos);
    int colour = pos.sideToMove;
    int bestScore = -INFINITY_SCORE;
    Move bestMove;
    SearchResult res;

    // Move ordering setup
    ScoredMove priorityMoveList[256];
    int n = 0;

    // Reorder move list based on priority factors again
    // Only get legal moves
    for (const Move& move : moveList) {
        if (isMoveLegal(pos, move)) {
            priorityMoveList[n].move = move;
            priorityMoveList[n].priorityScore = movePriorityScore(pos, move);
            n++;
        }
    }

    if (n == 0) {
        res.bestMove = {-1, -1, EMPTY};
        res.score = isInCheck(pos, colour) ? -MATE_SCORE : 0;
        return res;
    }

    // Iterative deepening loop
    for (int i = 1; i <= depth; i++) {
        // Reset best move
        bestMove.from = -1;
        bestMove.to = -1;
        bestMove.promotionPiece = EMPTY;
        bestScore = -INFINITY_SCORE;
        int alpha = -INFINITY_SCORE;
        int beta = INFINITY_SCORE;
        int bestIndex = -1;

        sort(priorityMoveList, priorityMoveList + n, priorityComparisonRule);

        for (int j = 0; j < n; j++) {
            const Move& move = priorityMoveList[j].move;
            UndoInfo undo = makeMove(pos, move);
    
            // We don't need to check for checks because
            // we've guaranteed this move is legal
            int score = -search(pos, i - 1, 1, res.nodes, -beta, -alpha);
            priorityMoveList[j].priorityScore = score;
            if (score > bestScore) {
                alpha = score;
                bestScore = score;
                bestMove = move;
                bestIndex = j;
            }
    
            unmakeMove(pos, move, undo);
        }
        priorityMoveList[bestIndex].priorityScore = INFINITY_SCORE;
    }

    res.bestMove = bestMove;
    res.score = bestScore;
    return res;
}