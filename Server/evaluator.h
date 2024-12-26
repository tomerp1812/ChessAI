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
    public:
        Evaluator();
        ~Evaluator();
        double evaluate(posRepresent* representation, std::vector<Move>& optionalMoves);
        void reorderMoves(posRepresent *representation, std::vector<Move>& optionalMoves);
};


#endif