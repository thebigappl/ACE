#include <iostream>
#include "types.hpp"
#include "movegen.hpp"
#include "position.hpp"
#include "move.hpp"
#include "attacks.hpp"
#include "benchmark.hpp"
using namespace std;

void testBenchmark() {
    Position pos;
    initialiseStartingPosition(pos);

    benchmarkSearch(pos, 2);
    benchmarkSearch(pos, 3);
    benchmarkSearch(pos, 4);
    benchmarkSearch(pos, 5);
    benchmarkSearch(pos, 6);
    benchmarkSearch(pos, 7);
    benchmarkSearch(pos, 8);
    benchmarkSearch(pos, 9);
}

int main() {
    testBenchmark();

    return 0;
}