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
        ChessAi(PosRepresentations& posRep, Evaluator& evaluator);
        ~ChessAi();
        MoveVal search(posRepresent* board);
        std::string run(std::string state);
        double minValue(posRepresent* representation, double alpha, double beta, unsigned int depth, unsigned long long int hash);
        double maxValue(posRepresent* representation, double alpha, double beta, unsigned int depth, unsigned long long int hash);
        // double minValueCaptures(posRepresent *representation, double alpha, double beta, unsigned int depth, unsigned long long int hash);
        // double maxValueCaptures(posRepresent *representation, double alpha, double beta, unsigned int depth, unsigned long long int hash);
        bool terminate(std::vector<Move> optionalMoves, int depth);
        void save(const posRepresent* representation, posRepresent& sp);
        void update(posRepresent* representation, const Move& move);
        void restore(posRepresent* representation, const posRepresent& sp, const Move& move);
        std::string moveToString(Move move);
};

#endif