#include <iostream>
#include <cassert>
#include "types.hpp"
#include "position.hpp"
#include "move.hpp"
using namespace std;

// Test that incremental hashing is consistent with full pos hash calculation
void testPositionHash() {
    Position pos;

    initialiseStartingPosition(pos);
    makeMove(pos, {E2, E4});
    U64 incrementalHash = pos.hash;
    initialisePositionHash(pos);

    assert(incrementalHash == pos.hash);

    makeMove(pos, {E7, E6});
    makeMove(pos, {E4, E5});
    makeMove(pos, {D7, D5});
    makeMove(pos, {E5, D6});
    incrementalHash = pos.hash;
    initialisePositionHash(pos);
    
    assert(incrementalHash == pos.hash);

    cout << "testPositionHash passed!\n";
}

int main() {
    testPositionHash();

    cout << "All tests passed!\n";

    return 0;
}