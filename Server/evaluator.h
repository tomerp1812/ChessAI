#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "logic.h"
struct Move;
struct posRepresent;
#include <vector>
#include <unordered_map>


class Evaluator{
    private:
        std::unordered_map<int, int> pieceToVal;
        unsigned long long int fBB[6]; // friendly bit boards
        unsigned long long int eBB[6]; // enemy bit boards
        const double (*(*friends_mg_table))[64];
        const double (*(*enemies_mg_table))[64];
        const double (*(*friends_eg_table))[64];
        const double (*(*enemies_eg_table))[64];
        int phaseValues[5];
        bool midleGame;
        int coefficient;

        // past pawns tables
        const double* fppmg;
        const double* fppeg;
        const double* eppmg;
        const double* eppeg;

        // forward masks
        const unsigned long long int* ffm;
        const unsigned long long int* efm;

        double (Evaluator::*evalArr[5])(int position, unsigned long long int friends, unsigned long long int enemies);

    public:
        Evaluator();
        ~Evaluator();
        void initPieceToVal();
        void initPhaseVal();
        void initEvalArr();
        void setTables(char turn);
        double pawnEval(int totalPhase, unsigned long long int friendlyPawns, unsigned long long int enemyPawns, int coefficient, const double* mgPP, const double* egPP, const unsigned long long int* forwardMask, const unsigned long long int* backwardMask);
        double knightEval(int position, unsigned long long int friends, unsigned long long int enemies);
        double bishopEval(int position, unsigned long long int friends, unsigned long long int enemies);
        double rookEval(int position, unsigned long long int friends, unsigned long long int enemies);
        double queenEval(int position, unsigned long long int friends, unsigned long long int enemies);
        double kingEval(int position, unsigned long long int friends, unsigned long long int enemies);
        double evaluate(const posRepresent* representation, const std::vector<Move>& optionalMoves);
        void reorderMoves(posRepresent *representation, std::vector<Move>& optionalMoves);
        // bool static compareByGuessValue(const Move &a, const Move &b);
};


#endif