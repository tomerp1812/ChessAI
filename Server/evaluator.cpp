#include "evaluator.h"
#include <limits>
#include <iostream>
#include "PosRepresentations.h"

Evaluator::Evaluator(){
    this->pieceToVal[1] = 100; // white pawn
    this->pieceToVal[2] = 290; // white knight
    this->pieceToVal[3] = 300; // white bishop
    this->pieceToVal[4] = 500; // white rook
    this->pieceToVal[5] = 900; // white queen
    this->pieceToVal[6] = 10000000; // white king
    this->pieceToVal[-1] = 100;// black pawn
    this->pieceToVal[-2] = 290;// black knight
    this->pieceToVal[-3] = 300;// black bishop
    this->pieceToVal[-4] = 500;// black rook
    this->pieceToVal[-5] = 900;// black queen
    this->pieceToVal[-6] = 10000000;// black king
    this->pieceToVal[0] = 0; // empty square
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
        val += this->pieceToVal[representation->board[position]];
        friends &= ~(1ULL << position);
    }

    while(enemies){
        int position = __builtin_ctzll(enemies);
        val -= this->pieceToVal[representation->board[position]];
        enemies &= ~(1ULL << position);
    }

    return val;
    
}

void Evaluator::reorderMoves(posRepresent *representation, std::vector<Move>& optionalMoves){

}