#include <iostream>
#include <cassert>
#include "types.hpp"
#include "position.hpp"
#include "move.hpp"
using namespace std;

// Test that incremental hashing is consistent with full pos hash calculation
void testIncrementalHash() {
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

void testPolyglotHash() {
    Position pos;

    initialiseStartingPosition(pos);
    assert(pos.hash == 0x463B96181691FC9C);

    makeMove(pos, {E2, E4});
    assert(pos.hash == 0x823C9B50FD114196);

    makeMove(pos, {D7, D5});
    assert(pos.hash == 0x0756B94461C50FB0);

    makeMove(pos, {E4, E5});
    assert(pos.hash == 0x662FAFB965DB29D4);

    makeMove(pos, {F7, F5});
    assert(pos.hash == 0x22A48B5A8E47FF78);

    makeMove(pos, {E1, E2});
    assert(pos.hash == 0x652A607CA3F242C1);

    cout << "testPolyglotHash passed!\n";
}

int main() {
    testIncrementalHash();
    testPolyglotHash();

    cout << "All tests passed!\n";

    return 0;
}