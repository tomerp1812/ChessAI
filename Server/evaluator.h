#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "logic.h"
struct Move;
struct posRepresent;
#include <vector>

class Evaluator{
    private:

    public:
        Evaluator();
        ~Evaluator();
        double evaluate(posRepresent* representation, std::vector<Move>& optionalMoves);
        void reorderMoves(posRepresent *representation, std::vector<Move>& optionalMoves);
};


#endif