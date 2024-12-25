#include "ChessAi.h"
#include<limits>
#include <iostream>
#include <string>
#include <vector>
#include "Logic.h"
#include "PosRepresentations.h"


using namespace std;


ChessAi::ChessAi(PosRepresentations posRep){
    this->posRep = &posRep;
    //implementation
}

ChessAi::~ChessAi(){
    //implementation
}

double ChessAi::evaluate(){
    //implementation
    return 0.0;
}

bool ChessAi::terminate(std::vector<Move> optionalMoves, int depth){
    if(depth <= 0 || optionalMoves.size() == 0){
        return true;
    }
    return false;
}

MoveVal ChessAi::search(posRepresent* representation){
    double alpha = -1 * numeric_limits<double>::infinity();
    double beta = numeric_limits<double>::infinity();
    double value = -1 * numeric_limits<double>::infinity();
    int depth = 3;
    Logic* logic = new Logic();
    vector<Move> optionalMoves = logic->getOptionalMoves(representation);
    cout << optionalMoves.size() << endl;
    MoveVal moveVal;
    moveVal.move = "";
    for(Move move: optionalMoves){
        double val = minValue(representation, alpha, beta, depth);

        if(moveVal.move == ""){
            moveVal.move = "somestring"; // change!!
            moveVal.value = val;

        }else if(moveVal.value < val){
            moveVal.move = "somestringElse"; // change!!!
            moveVal.value = val;
        }
        if(alpha < moveVal.value){
            alpha = moveVal.value;
        }
    }


    delete logic;
    return moveVal;
}

double ChessAi::maxValue(posRepresent* representation, double alpha, double beta, unsigned int depth){
    double value = -1 * numeric_limits<double>::infinity();
    Logic* logic = new Logic();
    vector<Move> optionalMoves = logic->getOptionalMoves(representation);

    if(terminate(optionalMoves, depth)){
        return evaluate();
    }

    for(Move move: optionalMoves){
        double val = minValue(representation, alpha, beta, depth - 1);
        if(val >= value){
            value = val;
        }
        if(value >= beta){
            delete logic;
            return value;
        }
        if(value >= alpha){
            alpha = value;
        }
    }
    
    delete logic;
    return value;
    
}

double ChessAi::minValue(posRepresent* representation, double alpha, double beta, unsigned int depth){
    double value = numeric_limits<double>::infinity();
    Logic* logic = new Logic();
    vector<Move> optionalMoves = logic->getOptionalMoves(representation);

    if(terminate(optionalMoves, depth)){
        return evaluate();
    }

    for(Move move: optionalMoves){
        double val = maxValue(representation, alpha, beta, depth - 1);
        if(val >= value){
            value = val;
        }
        if(value <= alpha){
            delete logic;
            return value;
        }
        if(beta <= value){
            beta = value;
        }
    }
    
    delete logic;
    return value;
}

string ChessAi::run(string state){
    posRepresent* representation = this->posRep->fenToBoard(state);
    string bestMove = search(representation).move;
    delete representation->board;
    delete representation->castle;
    delete representation;
    return bestMove;
}