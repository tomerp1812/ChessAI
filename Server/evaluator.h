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

    public:
        Evaluator();
        ~Evaluator();
        void initPieceToVal();
        void initPhaseVal();
        void setTables(posRepresent *representation);
        double evaluate(posRepresent* representation, std::vector<Move>& optionalMoves);
        void reorderMoves(posRepresent *representation, std::vector<Move>& optionalMoves);
        // bool static compareByGuessValue(const Move &a, const Move &b);
};


#endif