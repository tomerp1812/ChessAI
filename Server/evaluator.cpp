#include "evaluator.h"
#include <limits>
#include "PosRepresentations.h"


Evaluator::Evaluator(){

}

Evaluator::~Evaluator(){

}

double Evaluator::evaluate(posRepresent *representation, std::vector<Move>& optionalMoves){
    if(optionalMoves.size() == 0){
        return -1 * std::numeric_limits<double>::infinity();
    }

    double val = 0;
    unsigned long long int friends = representation->friends;
    unsigned long long int enemies = representation->enemies;

    while(friends){
        int position = __builtin_ctzll(friends);
        val += representation->board[position];
        friends &= ~(1ULL << position);
    }

    while(enemies){
        int position = __builtin_ctzll(enemies);
        val += representation->board[position];
        enemies &= ~(1ULL << position);
    }

    return val;
    
}