#pragma once

#include <optional>
#include <fstream>
#include <random>
#include "types.hpp"
#include "constants.hpp"
#include "movegen.hpp"
using namespace std;

class OpeningBook
{
private:
    /* data */
    vector<BookEntry> entries;
    bool loaded = false;
    random_device rd;
    mt19937_64 rng{rd()};

    pair<size_t, size_t> findBookMoves(const U64 targetHash) const;
    Move decodeMove(const Position& pos, U16 polyglotMove) const;
    Move chooseWeightedMove(const vector<Move>& candidateMoves,
                            const vector<U16>& candidateWeights);

public:
    bool loadBook(const string& filePath);
    bool isLoaded() const;
    void clearBook();
    optional<Move> getBookMove(const Position& pos);
};
