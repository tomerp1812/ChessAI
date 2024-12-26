#ifndef CHESSAI_H
#define CHESSAI_H

#include <string>
#include <vector>
#include "Logic.h"

class PosRepresentations;
class Evaluator;
struct posRepresent;
struct Move;

struct MoveVal
{
    double value;
    Move move;
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
        PosRepresentations* posRep;
        Evaluator* evaluator;
    public:
        ChessAi(PosRepresentations& posRep, Evaluator& evaluator);
        ~ChessAi();
        MoveVal search(posRepresent* board);
        std::string run(std::string state);
        double minValue(posRepresent* representation, double alpha, double beta, unsigned int depth);
        double maxValue(posRepresent* representation, double alpha, double beta, unsigned int depth);
        bool terminate(std::vector<Move> optionalMoves, int depth);
        void save(posRepresent* representation, Move move, savePosition& sp);
        void update(posRepresent* representation, Move move);
        void restore(posRepresent* representation, savePosition& sp);
        std::string moveToString(Move move);
};

#endif