#ifndef CHESSAI_H
#define CHESSAI_H
#include <string>
#include <iostream>
#include "PosRepresentations.h"

struct MoveVal
{
    double value;
    std::string move;
};



class ChessAi {
    private:
        PosRepresentations* posRep;
    public:
        ChessAi(PosRepresentations posRep);
        ~ChessAi();
        double evaluate();
        MoveVal search(int* board);
        std::string run(std::string state);
        double minValue(double alpha, double beta, unsigned int depth);
        double maxValue(double alpha, double beta, unsigned int depth);
        bool terminate(vector<Move> optionalMoves, int depth);
};

#endif