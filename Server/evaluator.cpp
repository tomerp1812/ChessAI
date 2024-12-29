#include "evaluator.h"
#include <limits>
#include <iostream>
#include <algorithm>
#include "PosRepresentations.h"
#include "lookup.h"

// Function declaration
bool compareByGuessValue(const Move &a, const Move &b);

Evaluator::Evaluator(){
    initPieceToVal();
    initPhaseVal();
}


Evaluator::~Evaluator(){

}

void Evaluator::initPieceToVal(){
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


// phase values are an estimation of whether position is midle or late game.
void Evaluator::initPhaseVal(){
    this->phaseValues[0] = 1; // pawn
    this->phaseValues[1] = 1; // knight
    this->phaseValues[2] = 1; // bishop
    this->phaseValues[3] = 2; // rook
    this->phaseValues[4] = 4; // queen
    this->phaseValues[5] = 0;
}

void Evaluator::setTables(posRepresent *representation){
    if(representation->turn == 'w'){
        this->friends_mg_table = white_mg_table;
        this->enemies_mg_table = black_mg_table;
        this->friends_eg_table = white_eg_table;
        this->enemies_eg_table = black_eg_table;
    }else{
        this->friends_mg_table = black_mg_table;
        this->enemies_mg_table = white_mg_table;
        this->friends_eg_table = black_eg_table;
        this->enemies_eg_table = white_eg_table;
    }
}


double Evaluator::evaluate(posRepresent *representation, std::vector<Move>& optionalMoves){

    // init bitboards
    for(int i = 0; i < 6; i++){
        this->fBB[i] = 0;
        this->eBB[i] = 0;
    }

    double val = 0;
    double mgPositionsVals = 0;
    double egPositionsVals = 0;
    int totalPhase = 0;
    unsigned long long int friends = representation->friends;
    unsigned long long int enemies = representation->enemies;

    setTables(representation);

    while(friends){
        // fetching the position
        int position = __builtin_ctzll(friends);
        // value of friendly piece
        val += this->pieceToVal[representation->board[position]];
        int pieceInMyDictionary = abs(representation->board[position]) - 1;
        this->fBB[pieceInMyDictionary] |= (1ULL << position);

        // calculating in which phase we are
        totalPhase += this->phaseValues[pieceInMyDictionary];

        // evaluate pieces positions
        mgPositionsVals += (*this->friends_mg_table[pieceInMyDictionary])[position];
        egPositionsVals += (*this->friends_eg_table[pieceInMyDictionary])[position];

        friends &= ~(1ULL << position);
    }

    while(enemies){
        // fetching the position
        int position = __builtin_ctzll(enemies);
        // value of enemy piece
        val -= this->pieceToVal[representation->board[position]];
        int pieceInMyDictionary = abs(representation->board[position]) - 1;
        this->eBB[pieceInMyDictionary] |= (1ULL << position);

        // calculating in which phase we are
        totalPhase += this->phaseValues[pieceInMyDictionary];

        // evaluate pieces positions
        mgPositionsVals -= (*this->enemies_mg_table[pieceInMyDictionary])[position];
        egPositionsVals -= (*this->enemies_eg_table[pieceInMyDictionary])[position];

        enemies &= ~(1ULL << position);
    }
    
    totalPhase = std::min(totalPhase, 24);
    val += (mgPositionsVals * (double(totalPhase) / 24)) + (egPositionsVals * (1 - (double(totalPhase) / 24)));

    return val;
    
}

// not part of the Evaluator class, just a comperison funciton
bool compareByGuessValue(const Move &a, const Move &b){
    return a.guessedValue > b.guessedValue;
}

void Evaluator::reorderMoves(posRepresent *representation, std::vector<Move>& optionalMoves){


    setTables(representation);

    unsigned long long int blockers = representation->blockers;
    int totalPhase = 0;

    while(blockers){
        int position = __builtin_ctzll(blockers);
        int pieceInMyDictionary = abs(representation->board[position]) - 1;

        // calculating in which phase we are
        totalPhase += this->phaseValues[pieceInMyDictionary];

        blockers &= ~(1ULL << position);
    }

    totalPhase = std::min(totalPhase, 24);

    for(Move& move: optionalMoves){
        double guessedValue = 0.0;
        // better to capture opponent pieces
        guessedValue += this->pieceToVal[representation->board[move.targetPos]];

        // promotions better be checked
        if(move.promotedPiece != 0){
            guessedValue += this->pieceToVal[move.promotedPiece] - 100;
        }

        int pieceInDict = abs(representation->board[move.startPos]) - 1;

        // prefer moving to better squares
        guessedValue += ((*this->friends_mg_table[pieceInDict])[move.targetPos] * (double(totalPhase) / 24));
        guessedValue += ((*this->friends_eg_table[pieceInDict])[move.targetPos] * (1 - (double(totalPhase) / 24)));
  
        move.guessedValue = guessedValue;
    }
    
    // reordering the moves from better moves to worse, to prune more moves
    std::sort(optionalMoves.begin(), optionalMoves.end(), compareByGuessValue);
}






