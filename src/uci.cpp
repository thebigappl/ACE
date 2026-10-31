#include <iostream>
#include <string>
#include "types.hpp"
#include "board.hpp"
#include "position.hpp"
#include "search.hpp"
#include "movegen.hpp"
#include "pieces.hpp"
#include "move.hpp"
#include "constants.hpp"
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

string squareToStr(Square square) {
    string squareStr = "";
    squareStr += ('a' + squareFile[square]);
    squareStr += ('1' + squareRank[square]);

    return squareStr;
}

Square strToSquare(string squareStr) {
    return A1 + (squareStr[1] - '1') * 10 + (squareStr[0] - 'a');
}

string pieceTypeToStr(int pieceType) {
    if (pieceType == KNIGHT) return "n";
    if (pieceType == BISHOP) return "b";
    if (pieceType == ROOK) return "r";
    if (pieceType == QUEEN) return "q";
    return "";
}

int strToPieceType(string pieceTypeStr) {
    if (pieceTypeStr == "n") return KNIGHT;
    if (pieceTypeStr == "b") return BISHOP;
    if (pieceTypeStr == "r") return ROOK;
    if (pieceTypeStr == "q") return QUEEN;
    return NONE;
}

void runUci() {
    string input;
    Position pos;
    initialiseStartingPosition(pos);

    while (getline(cin, input)) {
        if (input == "uci") {
            cout << "id name " << NAME << "\n";
            cout << "id author " << AUTHOR << "\n";
            cout << "uciok\n";
        } else if (input == "isready") {
            cout << "readyok\n";
        } else if (input == "ucinewgame") {
            initialiseStartingPosition(pos);
        } else if (input.rfind("position ", 0) == 0) {
            istringstream iss(input);

            string command, option;
            if (!(iss >> command >> option) ||
                command != "position" ||
                option != "startpos") {
                continue;
            }
            Position tempPos;
            initialiseStartingPosition(tempPos);
            string moves;
            if (!(iss >> moves)) {
                pos = tempPos;
                continue;
            }
            if (moves != "moves") continue;

            bool valid = true;
            string moveStr;
            while (iss >> moveStr) {
                if (moveStr.length() != 4 && moveStr.length() != 5) {
                    valid = false;
                    break;
                }

                string fromStr = moveStr.substr(0, 2);
                string toStr = moveStr.substr(2, 2);

                Square from = strToSquare(fromStr);
                Square to = strToSquare(toStr);

                int promotionPiece;
                if (moveStr.length() == 4) {
                    promotionPiece = EMPTY;
                } else {
                    string pieceStr = moveStr.substr(4, 1);
                    int pieceType = strToPieceType(pieceStr);
                    if (pieceType == NONE) {
                        valid = false;
                        break;
                    }
                    promotionPiece = tempPos.pieces[from] + pieceType;
                }

                auto legalMoves = generateLegalMoves(tempPos);
                Move move{from, to, promotionPiece};
                auto it = find_if(legalMoves.begin(), legalMoves.end(), [&](const Move& candidate) {
                    return candidate.from == from && candidate.to == to && 
                        candidate.promotionPiece == promotionPiece;
                });

                if (it == legalMoves.end()) {
                    valid = false;
                    break;
                }

                makeMove(tempPos, move);
            }

            if (valid) pos = tempPos;

        } else if (input.rfind("go ", 0) == 0) {
            istringstream iss(input);

            string command, option;
            int depth = 8, wtime = -1, btime = -1, winc = 0, binc = 0, movestogo = 0;

            SearchLimits lims;

            if (!(iss >> command)) continue;
            bool valid = true;
            while (iss >> option) {
                if (option == "depth") {
                    if (!(iss >> depth) ||
                        depth < 1 ||
                        depth > 10) {
                        valid = false;
                        break;
                    }
                } else if (option == "wtime") {
                    if (!(iss >> wtime) ||
                        wtime < 0) {
                        valid = false;
                        break;
                    }
                } else if (option == "btime") {
                    if (!(iss >> btime) ||
                        btime < 0) {
                        valid = false;
                        break;
                    }
                } else if (option == "winc") {
                    if (!(iss >> winc) ||
                        winc < 0) {
                        valid = false;
                        break;
                    }
                } else if (option == "binc") {
                    if (!(iss >> binc) ||
                        binc < 0) {
                        valid = false;
                        break;
                    }
                } else if (option == "movestogo") {
                    if (!(iss >> movestogo) ||
                        movestogo <= 0) {
                        valid = false;
                        break;
                    }
                } else if (option == "infinite") {
                    // infinite is valid but doesn't do anything
                    // special for now
                } else {
                    valid = false;
                    break;
                }
            }
            
            if (!valid) continue;

            cout << "bestmove ";

            auto legalMoveList = generateLegalMoves(pos);
            if (legalMoveList.count == 0) {
                cout << "0000\n";
            } else {
                Move bestMove = findBestMove(pos, depth).bestMove;
                cout << squareToStr(bestMove.from) << squareToStr(bestMove.to);
                
                if (bestMove.promotionPiece != EMPTY) {
                    cout << pieceTypeToStr(pieceType[bestMove.promotionPiece]);
                }

                cout << '\n';
            }
        } else if (input == "quit") {
            break;
        }

        cout.flush();
    }
}