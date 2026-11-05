#include "types.hpp"
#include "constants.hpp"

bool isStartingPosition(int colour, int rank) {
    return ((colour == WHITE && rank == RANK_2) ||
            (colour == BLACK && rank == RANK_7));
}

bool isPromotionRank(int colour, int rank) {
    return ((colour == WHITE && rank == RANK_8) || 
            (colour == BLACK && rank == RANK_1));
}

int getM64Index(Square square) {
    return (squareRank[square] * 8 + squareFile[square]);
}
