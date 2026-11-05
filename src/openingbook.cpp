#include <optional>
#include <fstream>
#include <random>
#include "types.hpp"
#include "constants.hpp"
#include "movegen.hpp"
#include "openingbook.hpp"
using namespace std;

pair<size_t, size_t> OpeningBook::findBookMoves(const U64 targetHash) const {
    pair<size_t, size_t> range;
    size_t low = 0, high = entries.size();
    while (low < high) {
        size_t mid = (low + high) / 2;
        if (entries[mid].hash < targetHash) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }
    range.first = low;
    low = 0, high = entries.size();
    while (low < high) {
        size_t mid = (low + high) / 2;
        if (entries[mid].hash <= targetHash) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }
    range.second = low;
    return range;
}

Move OpeningBook::decodeMove(const Position& pos, U16 polyglotMove) const {
    Move move;
    move.from = m120Square[(polyglotMove >> SRC_SHIFT) & SQUARE_MASK];

    int to = m120Square[(polyglotMove >> DEST_SHIFT) & SQUARE_MASK];

    // If castle
    int dist = abs(to - move.from);
    if (pieceType[move.from] == KING && (dist == 4 || dist == 3)) {
        if (to - move.from > 0) {
            to = move.from + 2;
        } else {
            to = move.from - 2;
        }
    }
    move.to = to;
    int promo = (polyglotMove >> PROMO_SHIFT) & PROMO_MASK;
    if (promo != 0) {
        move.promotionPiece = pos.pieces[move.from] + promo;
    } else {
        move.promotionPiece = EMPTY;
    }

    return move;
}

Move OpeningBook::chooseWeightedMove(const vector<Move>& candidateMoves,
                                    const vector<U16>& candidateWeights) {
    U64 totalWeight = 0;
    U64 n = 0;
    for (auto weight : candidateWeights) {
        totalWeight += weight;
        n++;
    }
    if (totalWeight == 0) {
        uniform_int_distribution<U64> dist(0, n - 1);
        U64 i = dist(rng);
        return candidateMoves[i];
    }

    U64 currWeight = 0;
    uniform_int_distribution<U64> dist(0, totalWeight - 1);
    U64 randomValue = dist(rng);
    for (U64 i = 0; i < n; i++) {
        currWeight += candidateWeights[i];
        if (currWeight < randomValue) {
            return candidateMoves[i];
        }
    }
    
    return candidateMoves[0];
}

bool OpeningBook::loadBook(const string& filePath) {
    ifstream Book(filePath, ios::binary);
    if (!Book.is_open()) return false;

    Book.seekg(0, ifstream::end);
    if (!Book) return false;

    streampos fileSize = Book.tellg();
    if (fileSize == streampos(-1)) return false;

    streamsize numBytes = static_cast<streamsize>(fileSize);
    if (numBytes % 16 != 0) return false;
    Book.seekg(0, ifstream::beg);

    size_t numEntries = static_cast<size_t>(numBytes / 16);
    vector<BookEntry> temp(numEntries);

    for (size_t i = 0; i < numEntries; i++) {
        array<U8, 16> buf;
        Book.read(reinterpret_cast<char *>(buf.data()), 16);

        if (!Book) return false;

        BookEntry entry;
        for (int j = 0; j < 8; j++) {
            entry.hash |= ((U64)buf[HASH_OFFSET + j] << (7 - j) * 8);
        }
        for (int j = 0; j < 2; j++) {
            entry.move |= ((U16)buf[MOVE_OFFSET + j] << (1 - j) * 8);
        }
        for (int j = 0; j < 2; j++) {
            entry.weight |= ((U16)buf[WEIGHT_OFFSET + j] << (1 - j) * 8);
        }
        for (int j = 0; j < 4; j++) {
            entry.learn |= ((U32)buf[LEARN_OFFSET + j] << (3 - j) * 8);
        }

        temp[i] = entry;
    }

    entries = move(temp);
    loaded = true;
    return true;
}

bool OpeningBook::isLoaded() const {
    return loaded;
}

void OpeningBook::clearBook() {
    entries.clear();
    loaded = false;
}

optional<Move> OpeningBook::getBookMove(const Position& pos) {
    if (!loaded) return nullopt;

    pair<size_t, size_t> range = findBookMoves(pos.hash);
    if (range.first == range.second) return nullopt;

    MoveList legalMoves = generateLegalMoves(pos);
    vector<Move> candidateMoves;
    vector<U16> candidateWeights;
    for (size_t i = range.first; i < range.second; i++) {
        Move temp = decodeMove(pos, entries[i].move);
        bool found = false;
        for (Move& move : legalMoves) {
            if (move == temp) {
                found = true;
                break;
            }
        }
        if (!found) continue;

        candidateMoves.push_back(temp);
        candidateWeights.push_back(entries[i].weight);
    }

    if (candidateMoves.empty()) return nullopt;

    return chooseWeightedMove(candidateMoves, candidateWeights);
}
