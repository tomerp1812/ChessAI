#ifndef CHESSAI_H
#define CHESSAI_H

#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>
#include "Logic.h"

class PosRepresentations;
class Evaluator;
class ZobristHash;
struct posRepresent;
struct Move;

struct MoveVal
{
    Move move;
    double value;
    unsigned int depth;
    unsigned int mateIn = 0;
};

struct savePosition
{
    Move move;
    int piece;
    int otherPiece;
    int otherPiecePosition;
    int myKing;
    int oppKing;
    unsigned long long int blockers;
    unsigned long long int friends;
    unsigned long long int enemies;
    char turn; // w is white, b is black
    char castle[4];
    int enPassant;
    int halfMove;
    int fullMove;
};

class ChessAi {
    private:
        int counter;
        unsigned long long int newPositionHash;
        unsigned int startingDepth;
        unsigned int totalDepth;
        unsigned int halfDepth;
        const unsigned int timeLimit = 2000;
        std::chrono::steady_clock::time_point startTime;
        std::unordered_map<unsigned long long int, double> transpositionTable;
        PosRepresentations* posRep;
        Evaluator* evaluator;
        Logic* logic;
        ZobristHash* zobrist;
    public:
        ChessAi();
        ~ChessAi();
        MoveVal iddfs(posRepresent* representation);
        MoveVal search(posRepresent* representation, const std::vector<Move>& optionalMoves, unsigned long long int hash);
        std::string run(std::string state);
        MoveVal negaMax(posRepresent *representation, unsigned int depth, double alpha, double beta, unsigned long long int hash);
        MoveVal quiescence(posRepresent *representation, unsigned int depth, double alpha, double beta, unsigned long long int hash);
        bool terminate(std::vector<Move> optionalMoves, int depth);
        void save(const posRepresent* representation, posRepresent& sp);
        unsigned long long int update(posRepresent* representation, const Move& move, unsigned long long int hash);
        void restore(posRepresent* representation, const posRepresent& sp, const Move& move);
        std::string moveToString(Move move);
};

#endif