#ifndef CHESSAI_H
#define CHESSAI_H

#include <string>
#include <vector>
#include <unordered_map>
#include "Logic.h"

class PosRepresentations;
class Evaluator;
class ZobristHash;
struct posRepresent;
struct Move;

struct MoveVal
{
    double value;
    Move move;
    unsigned int depth;
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
        std::unordered_map<unsigned long long int, double> transpositionTable;
        PosRepresentations* posRep;
        Evaluator* evaluator;
        Logic* logic;
        ZobristHash* zobrist;
    public:
        ChessAi();
        ~ChessAi();
        MoveVal search(posRepresent* board);
        std::string run(std::string state);
        MoveVal negaMax(posRepresent *representation, unsigned int depth, double alpha, double beta, unsigned long long int hash);
        double quiescence(posRepresent *representation, unsigned int depth, double alpha, double beta, unsigned long long int hash);
        bool terminate(std::vector<Move> optionalMoves, int depth);
        void save(const posRepresent* representation, posRepresent& sp);
        unsigned long long int update(posRepresent* representation, const Move& move, unsigned long long int hash);
        void restore(posRepresent* representation, const posRepresent& sp, const Move& move);
        std::string moveToString(Move move);
};

#endif