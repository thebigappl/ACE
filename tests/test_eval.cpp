#include <iostream>
#include <cassert>
#include "types.hpp"
#include "position.hpp"
#include "evaluation.hpp"
using namespace std;

void testBasicEvaluation() {
    Position pos;
    initialiseStartingPosition(pos);

    assert(evaluatePosition(pos) == 0);

    cout << "testBasicEvaluation passed!\n";
}

int main() {
    testBasicEvaluation();

    cout << "All tests passed!\n";

    return 0;
}