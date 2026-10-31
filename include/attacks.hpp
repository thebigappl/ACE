#pragma once

#include "types.hpp"

bool isDiagonalDirection(int direction);

// Position validation functions
bool isPawnAttack(const Position& pos, int square, int attackingColour);
bool isKnightAttack(const Position& pos, int square, int attackingColour);
bool isSlidingAttack(const Position& pos, int square, int attackingColour);
bool isKingAttack(const Position& pos, int square, int attackingColour);
bool isSquareAttacked(const Position& pos, int square, int attackingColour);

bool isInCheck(const Position& pos, int colour);
bool isCheckmate(const Position& pos);
bool isStalemate(const Position& pos);
bool isFiftyMoveDraw(const Position& pos);
bool isCapture(const Position& pos, const Move& move);
bool isCastleLegal(Position& pos, const Move& move);
bool isMoveLegal(Position& pos, const Move& move);