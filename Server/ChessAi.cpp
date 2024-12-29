#include "ChessAi.h"
#include <limits>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include "Logic.h"
#include "PosRepresentations.h"
#include "evaluator.h"
#include "zobristHash.h"

using namespace std;
using namespace chrono;

ChessAi::ChessAi(PosRepresentations& posRep, Evaluator& evaluator)
{
    this->posRep = &posRep;
    this->evaluator = &evaluator;
    this->logic = new Logic();
    this->zobrist = new ZobristHash();
}

ChessAi::~ChessAi()
{
    delete this->logic;
    delete this->zobrist;
}

bool ChessAi::terminate(std::vector<Move> optionalMoves, int depth)
{
    if (depth <= 0 || optionalMoves.size() == 0)
    {
        return true;
    }
    return false;
}

void ChessAi::save(const posRepresent *representation, posRepresent& sp)
{
    sp.myKing = representation->myKing;
    sp.oppKing = representation->oppKing;
    sp.blockers = representation->blockers;
    sp.friends = representation->friends;
    sp.enemies = representation->enemies;
    sp.turn = representation->turn;
    sp.enPassant = representation->enPassant;
    sp.halfMove = representation->halfMove;
    sp.fullMove = representation->fullMove;

    for (int i = 0; i < 4; i++)
    {
        sp.castle[i] = representation->castle[i];
    }

    for(int i = 0; i < 64; i++){
        sp.board[i] = representation->board[i];
    }

}

void ChessAi::update(posRepresent *representation, const Move& move)
{
    representation->halfMove++;
    if(representation->board[move.targetPos] != 0){
        representation->halfMove = 0;
    }

    
    int piece = representation->board[move.startPos];
    
    representation->enPassant = -1;
    representation->board[move.targetPos] = representation->board[move.startPos];
    representation->board[move.startPos] = 0;

    representation->friends &= ~(1ULL << move.startPos);
    representation->friends |= (1ULL << move.targetPos);

    // king
    if (abs(piece) == 6)
    {
        representation->myKing = move.targetPos;
        // can't castle after that move
        if(representation->turn == 'w'){
            representation->castle[0] = '.';
            representation->castle[1] = '.';
        }else{
            representation->castle[2] = '.';
            representation->castle[3] = '.';
        }
        // check if the move is castling
        if (abs((move.targetPos - move.startPos)) == 2)
        {
            int startRook;
            int endRook;
            // king side castle
            if (move.startPos < move.targetPos)
            {
                startRook = move.targetPos + 1;
                endRook = move.startPos + 1;
                int rook = representation->board[move.startPos + 3];
                representation->board[move.startPos + 3] = 0;
                representation->board[move.startPos + 1] = rook;
                
            }
            // queen side castle
            else
            {
                startRook = move.targetPos - 2;
                endRook = move.startPos - 1;
                int rook = representation->board[move.startPos - 4];
                representation->board[move.startPos - 4] = 0;
                representation->board[move.startPos - 1] = rook;
            }
            representation->friends &= ~(1ULL << startRook);
            representation->friends |= (1ULL << endRook);
        }
    }
    // pawn
    else if (abs(piece) == 1)
    {
        representation->halfMove = 0;
        // first move 2 squares of pawn
        if (abs((move.targetPos - move.startPos)) == 16)
        {
            if(representation->turn == 'w'){
                representation->enPassant = move.startPos + 8;
            }else{
                representation->enPassant = move.startPos - 8;
            }
            
        }
        // check if was en-passant capture
        else if(representation->enPassant == move.targetPos){
            if(representation->turn == 'w'){
                representation->board[move.targetPos - 8] = 0;
                representation->enemies &= ~(1ULL << (move.targetPos - 8));
            }else{
                representation->board[move.targetPos + 8] = 0;
                representation->enemies &= ~(1ULL << (move.targetPos + 8));
            }
        // promotion
        }else if(move.promotedPiece != 0){
            representation->board[move.targetPos] = move.promotedPiece;
        }
    }
    // rook
    else if(abs(piece) == 4){
        // rook move needs to disable castling to this side
        if(representation->turn == 'w'){
            if(move.startPos == 0){
                representation->castle[1] = '.';
            }else if(move.startPos == 7){
                representation->castle[0] = '.';
            }
        }else{
            if(move.startPos == 56){
                representation->castle[3] = '.';
            }else if(move.startPos == 63){
                representation->castle[2] = '.';
            }
        }
    }

    // capture a potential rook, not able to castle anymore
    if(move.targetPos == 0){
        representation->castle[1] = '.';
    }else if(move.targetPos == 7){
        representation->castle[0] = '.';
    }else if(move.targetPos == 56){
        representation->castle[3] = '.';
    }else if(move.targetPos == 63){
        representation->castle[2] = '.';
    }

    // if otherpiece is captured
    representation->enemies &= ~(1ULL << move.targetPos);

    if (representation->turn == 'w')
    {
        representation->turn = 'b';
    }
    else
    {
        representation->turn = 'w';
        representation->fullMove += 1;
    }

    representation->blockers = (representation->friends | representation->enemies);

    int tmpKing = representation->myKing;
    representation->myKing = representation->oppKing;
    representation->oppKing = tmpKing;

    unsigned long long int friends = representation->friends;
    representation->friends = representation->enemies;
    representation->enemies = friends;
}

void ChessAi::restore(posRepresent* representation,const posRepresent& sp, const Move& move){
    representation->turn = sp.turn;
    representation->halfMove = sp.halfMove;
    representation->fullMove = sp.fullMove;
    representation->enPassant = sp.enPassant;
    for(int i = 0; i < 4; i++){
        representation->castle[i] = sp.castle[i];
    }
    representation->friends = sp.friends;
    representation->enemies = sp.enemies;
    representation->blockers = sp.blockers;

    representation->myKing = sp.myKing;
    representation->oppKing = sp.oppKing;

    representation->board[move.startPos] = sp.board[move.startPos];
    representation->board[move.targetPos] = sp.board[move.targetPos];

    // in case of en-passant
    representation->board[move.startPos + 1] = sp.board[move.startPos + 1];
    representation->board[move.startPos - 1] = sp.board[move.startPos - 1];

    // in case of castling, short castle
    representation->board[move.targetPos + 1] = sp.board[move.targetPos + 1];
    // long castle
    representation->board[move.targetPos - 2] = sp.board[move.targetPos - 2];

}

MoveVal ChessAi::search(posRepresent *representation)
{
    this->counter = 0;
    auto start = high_resolution_clock::now();
    double alpha = -1 * numeric_limits<double>::infinity();
    double beta = numeric_limits<double>::infinity();
    int depth = 5;
    
    vector<Move> optionalMoves = this->logic->getOptionalMoves(representation, false);
    this->evaluator->reorderMoves(representation, optionalMoves);

    // Initialize Zobrist hash
    unsigned long long int hash = zobrist->computeZobristHash(*representation);
     // Track initial position
    zobrist->recordPosition(hash);

    unsigned long long int updatedHash;

    MoveVal moveVal;
    moveVal.value = -1 * numeric_limits<double>::infinity();

    posRepresent sp;
    save(representation, sp);

    for (Move move : optionalMoves)
    {
        this->counter++;

        // Update Zobrist hash for the new position
        updatedHash = zobrist->updateZobristHash(hash, *representation, move);
        zobrist->recordPosition(updatedHash);
        update(representation, move);
        double val;
        // Check for threefold repetition
        if (zobrist->isThreefoldRepetition(updatedHash))
        {
            val = 0;
        }else if(this->zobrist->transpositionExists(updatedHash)){
            val = this->zobrist->transpositionEval(updatedHash);
        }    
        else{
            val = minValue(representation, alpha, beta, depth, updatedHash);
            this->zobrist->saveTranspositionTable(updatedHash, val, depth);
        }
        restore(representation, sp, move);
        zobrist->undoPosition(updatedHash);
  
        // cout << "piece: " << representation->board[move.startPos] << " from: " << move.startPos 
        // << " to: "<< move.targetPos << " value: " << val << endl; 

        if (moveVal.value <= val)
        {
            moveVal.move = move;
            moveVal.value = val;
        }
        if (alpha < moveVal.value)
        {
            alpha = moveVal.value;
        }
        
        
    }
    auto end = high_resolution_clock::now();
    cout << duration_cast<seconds>(end - start).count() << '\n';

    // record the move
    zobrist->recordPosition(updatedHash); 
    this->zobrist->saveTranspositionTable(hash, moveVal.value, depth);
    return moveVal;
}

double ChessAi::maxValue(posRepresent *representation, double alpha, double beta, unsigned int depth, unsigned long long int hash)
{
    double value = -1 * numeric_limits<double>::infinity();
    vector<Move> optionalMoves = this->logic->getOptionalMoves(representation, false);
    

    if(optionalMoves.size() == 0){
        // lost - legalPosition returns false if in check
        if(!this->logic->legalPosition(representation)){
            return -value;
        }
        // tie
        return 0;
    }

    if (depth == 0)
    {
        // return maxValueCaptures(representation, alpha, beta, 3);
        return this->evaluator->evaluate(representation, optionalMoves);
    }

    this->evaluator->reorderMoves(representation, optionalMoves);

    unsigned long long int updatedHash;

    posRepresent sp;
    save(representation, sp);

    for (Move move : optionalMoves)
    {
        this->counter++;

        // Update Zobrist hash
        updatedHash = zobrist->updateZobristHash(hash, *representation, move);
        zobrist->recordPosition(updatedHash);

        double val;

        // Check for threefold repetition
        if (this->zobrist->isThreefoldRepetition(updatedHash))
        {
            val = 0;
        }else if(this->zobrist->transpositionExists(updatedHash)){
            val = this->zobrist->transpositionEval(updatedHash);
        }
        else{
            update(representation, move);
            val = minValue(representation, alpha, beta, depth - 1, updatedHash);
            this->zobrist->saveTranspositionTable(updatedHash, val, depth);

        }
        restore(representation, sp, move);
        zobrist->undoPosition(updatedHash);

        value = max(val, value);
        if (value >= beta)
        {
            return value;
        }
        alpha = max(alpha, value);
    }

    return value;
}

double ChessAi::minValue(posRepresent *representation, double alpha, double beta, unsigned int depth, unsigned long long int hash)
{
    double value = numeric_limits<double>::infinity();
    vector<Move> optionalMoves = logic->getOptionalMoves(representation, false);

    if(optionalMoves.size() == 0){
        // lost - legalPosition returns false if in check
        if(!this->logic->legalPosition(representation)){
            return value;
        }
        // tie
        return 0;
    }

    if (depth == 0)
    {
        // return minValueCaptures(representation, alpha, beta, 3);
        return this->evaluator->evaluate(representation, optionalMoves);
    }

    this->evaluator->reorderMoves(representation, optionalMoves);

    unsigned long long int updatedHash;
    posRepresent sp;
    save(representation, sp);

    for (Move move : optionalMoves)
    {
        this->counter++;  

        // Update Zobrist hash
        updatedHash = zobrist->updateZobristHash(hash, *representation, move);
        zobrist->recordPosition(updatedHash);
        double val;

         // Check for threefold repetition
        if (zobrist->isThreefoldRepetition(updatedHash))
        {
            val = 0;
        }else if(this->zobrist->transpositionExists(updatedHash)){
            val = this->zobrist->transpositionEval(updatedHash);
        }
        else{
            update(representation, move);
            val = maxValue(representation, alpha, beta, depth - 1, updatedHash);
            this->zobrist->saveTranspositionTable(updatedHash, val, depth);
        }

        restore(representation, sp, move);
        zobrist->undoPosition(updatedHash);

        value = min(val, value);
        if (value <= alpha)
        {
            return value;
        }
        beta = min(beta, value);
        
        
    }

    return value;
}

// same as minValue but only looking for captures and promotions
// double ChessAi::minValueCaptures(posRepresent *representation, double alpha, double beta, unsigned int depth){

//     vector<Move> optionalMoves = this->logic->getOptionalMoves(representation, true); // Only get capture moves
//     double value = this->evaluator->evaluate(representation, optionalMoves);

//     if(value <= alpha){
//         return value;
//     }

//     beta = min(beta, value);

//     if(depth == 0 || optionalMoves.size() == 0){
//         return value;
//     }

//     // if (optionalMoves.size() == 0)
//     // {
//     //     // No captures available, evaluate the position
//     //     return this->evaluator->evaluate(representation, optionalMoves);
//     // }

//     this->evaluator->reorderMoves(representation, optionalMoves);

//     savePosition sp;
//     for (Move move : optionalMoves)
//     {
//         this->counter++;
//         save(representation, move, sp);
//         update(representation, move);
//         double val = maxValueCaptures(representation, alpha, beta, depth - 1); // Continue recursively with capture evaluation
//         restore(representation, sp);
//         value = min(val, value);
//         if (value <= alpha)
//         {
//             return value;
//         }
//         beta = min(beta, value);
//     }

//     return value;
// }

// double ChessAi::maxValueCaptures(posRepresent *representation, double alpha, double beta, unsigned int depth){

//     vector<Move> optionalMoves;
//     if(depth == 0){
//         optionalMoves = this->logic->getOptionalMoves(representation, false);
//     }else{
//         optionalMoves = this->logic->getOptionalMoves(representation, true); // Only get capture moves
//     }
    
//     double value = this->evaluator->evaluate(representation, optionalMoves);

//     if(value >= beta){
//         return value;
//     }

//     alpha = max(alpha, value);

//     if(depth == 0 || optionalMoves.size() == 0){
//         return value;
//     }

//     // if (optionalMoves.size() == 0)
//     // {
//     //     // No captures available, evaluate the position
//     //     return this->evaluator->evaluate(representation, optionalMoves);
//     // }

//     this->evaluator->reorderMoves(representation, optionalMoves);

//     savePosition sp;
//     for (Move move : optionalMoves)
//     {
//         this->counter++;
//         save(representation, move, sp);
//         update(representation, move);
//         double val = minValueCaptures(representation, alpha, beta, depth - 1); // Continue recursively with capture evaluation
//         restore(representation, sp);
//         value = max(val, value);
//         if (value >= beta)
//         {
//             return value;
//         }
//         alpha = max(alpha, value);
//     }

//     return value;
// }


std::string ChessAi::moveToString(Move move) {
    auto toAlgebraic = [](int pos) {
        char file = 'a' + (pos % 8);    
        char rank = '1' + (pos / 8);
        return string(1, file) + string(1, rank);
    };

    std::string moveString = toAlgebraic(move.startPos) + toAlgebraic(move.targetPos);

    if(move.promotedPiece != 0){
        char promotion;
        switch(abs(move.promotedPiece)) {
            case 2: promotion = 'n';
                    break;
            case 3: promotion = 'b';
                    break;
            case 4: promotion = 'r';
                    break;
            case 5: promotion = 'q';
                    break;
            default: return moveString; // error no real promotion
        }

        moveString += promotion;
    }
    return moveString;
}

string ChessAi::run(string state)
{
    posRepresent *representation = this->posRep->fenToBoard(state);
    Move bestMove = search(representation).move;
    std::string moveString = moveToString(bestMove);
    std::cout << "searched:" <<this->counter << std::endl;
    // delete representation->board;
    // delete representation->castle;
    delete representation;

    return moveString;
}