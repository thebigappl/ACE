#include <chrono>
#include <iostream>
#include "types.hpp"
#include "search.hpp"
using namespace std;

void benchmarkSearch(Position& pos, int depth) {

    auto start = chrono::steady_clock::now();

    SearchResult res = findBestMove(pos, depth);

    auto end = chrono::steady_clock::now();

    double timeElapsed = chrono::duration<double>(end - start).count();

    double nps = timeElapsed > 0 ? res.nodes / timeElapsed : 0;

    cout << "Depth: " << depth << "\n";
    cout << "Time Elapsed: " << timeElapsed << "\n";
    cout << "Nodes: " << res.nodes << "\n";
    cout << "NPS: " << nps << "\n";
}
