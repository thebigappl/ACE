#include "types.hpp"
#include "movegen.hpp"
#include "attacks.hpp"
#include "move.hpp"

U64 perft(Position& pos, int depth) {
    if (depth == 0) return 1;

    U64 nodes = 0;
    auto moveList = generateMoves(pos);
    int colour = pos.sideToMove;
    int n = moveList.count;
    for (int i = 0; i < n; i++) {
        Move move = moveList.moves[i];
        UndoInfo undo = makeMove(pos, move);
        if (!isInCheck(pos, colour)) {
            nodes += (depth == 1) ? 1 : perft(pos, depth - 1);
        }
        unmakeMove(pos, move, undo);
    }

    return nodes;
}
