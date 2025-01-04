#include "ChessAi.h"
#include <limits>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include "Logic.h"
#include "lookup.h"
#include "PosRepresentations.h"
#include "evaluator.h"
#include "zobristHash.h"

using namespace std;
using namespace chrono;

ChessAi::ChessAi()
{    
    this->posRep = new PosRepresentations();
    this->evaluator = new Evaluator();
    this->logic = new Logic();
    this->zobrist = new ZobristHash();
}

ChessAi::~ChessAi()
{
    delete this->posRep;
    delete this->evaluator;
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

unsigned long long int ChessAi::update(posRepresent *representation, const Move& move, unsigned long long int hash)
{
    representation->halfMove++;
    // removing opponent captured piece
    if(representation->board[move.targetPos] != 0){
        int targetPiece = representation->board[move.targetPos];
        int targetIndex = (targetPiece > 0) ? targetPiece - 1 : 5 - targetPiece;
        hash ^= zobristTable[targetIndex][move.targetPos];
        representation->halfMove = 0;
    }

    // updating zobrist hash value
    int piece = representation->board[move.startPos];
    int pieceIndex = (piece > 0) ? piece - 1 : 5 - piece;
    hash ^= zobristTable[pieceIndex][move.startPos];
    // adding piece to new position
    hash ^= zobristTable[pieceIndex][move.targetPos];

    int lastMoveEnPassant = representation->enPassant;
    // remove old en-passant
    if(representation->enPassant != -1){
        int file = representation->enPassant % 8;
        hash ^= zobristEnPassant[file];
        representation->enPassant = -1;
    }
    


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
            // removing the castling for white
            if(representation->castle[0] != '.'){
                hash ^= zobristCastling[0];
                representation->castle[0] = '.';
            }
            if(representation->castle[1] != '.'){
                hash ^= zobristCastling[1];
                representation->castle[1] = '.';
            }     
        }else{
            // removing the castling for black
            if(representation->castle[2] != '.'){
                representation->castle[2] = '.';
                hash ^= zobristCastling[2];
            }
            if(representation->castle[3] != '.'){
                representation->castle[3] = '.';
                hash ^= zobristCastling[3];
            }            
        }
        // check if the move is castling
        if (abs((move.targetPos - move.startPos)) == 2)
        {
            int startRook;
            int endRook;
            int rookIndex;
            // king side castle
            if (move.startPos < move.targetPos)
            {
                startRook = move.targetPos + 1;
                endRook = move.startPos + 1;
                int rook = representation->board[move.startPos + 3];
                representation->board[move.startPos + 3] = 0;
                representation->board[move.startPos + 1] = rook;
                rookIndex = (rook > 0) ? rook - 1 : 5 - rook;
            }
            // queen side castle
            else
            {
                startRook = move.targetPos - 2;
                endRook = move.startPos - 1;
                int rook = representation->board[move.startPos - 4];
                representation->board[move.startPos - 4] = 0;
                representation->board[move.startPos - 1] = rook;
                rookIndex = (rook > 0) ? rook - 1 : 5 - rook;
            }
            representation->friends &= ~(1ULL << startRook);
            representation->friends |= (1ULL << endRook);

            // remove the rook from the hash and put it in new positon
            hash ^= zobristTable[rookIndex][startRook];
            hash ^= zobristTable[rookIndex][endRook];
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

            // updating en-passant for next move
            int file = representation->enPassant % 8;
            hash ^= zobristEnPassant[file];
        }
        // check if was en-passant capture
        else if(lastMoveEnPassant == move.targetPos){
            if(representation->turn == 'w'){
                // removing enemy pawn from hash
                hash ^= zobristTable[6][move.targetPos - 8];

                representation->board[move.targetPos - 8] = 0;
                representation->enemies &= ~(1ULL << (move.targetPos - 8));
            }else{
                // removing enemy pawn from the hash 
                hash ^= zobristTable[0][move.targetPos + 8];

                representation->board[move.targetPos + 8] = 0;
                representation->enemies &= ~(1ULL << (move.targetPos + 8));
            }
        // promotion
        }else if(move.promotedPiece != 0){
            representation->board[move.targetPos] = move.promotedPiece;

            //removing pawn and adding the promoted piece
            int promotedIndex = move.promotedPiece > 0 ? move.promotedPiece - 1 : 5 - move.promotedPiece;
            hash ^= zobristTable[pieceIndex][move.targetPos];
            hash ^= zobristTable[promotedIndex][move.targetPos];
        }
    }
    // rook
    else if(abs(piece) == 4){
        // rook move needs to disable castling to this side
        if(representation->turn == 'w'){
            if(move.startPos == 0){
                if(representation->castle[1] != '.'){
                    representation->castle[1] = '.';
                    hash ^= zobristCastling[1];
                }
            }else if(move.startPos == 7){
                if(representation->castle[0] != '.'){
                    representation->castle[0] = '.';
                    hash ^= zobristCastling[0];
                }
            }
        }else{
            if(move.startPos == 56){
                if(representation->castle[3] != '.'){
                    representation->castle[3] = '.';
                    hash ^= zobristCastling[3];
                }
            }else if(move.startPos == 63){
                if(representation->castle[2] != '.'){
                    representation->castle[2] = '.';
                    hash ^= zobristCastling[2];
                }
            }
        }
    }

    // capture a potential rook, not able to castle anymore
    if(move.targetPos == 0){
        if(representation->castle[1] != '.'){
            representation->castle[1] = '.';
            hash ^= zobristCastling[1];
        }
    }else if(move.targetPos == 7){
        if(representation->castle[0] != '.'){
            representation->castle[0] = '.';
            hash ^= zobristCastling[0];
        }
    }else if(move.targetPos == 56){
        if(representation->castle[3] != '.'){
            representation->castle[3] = '.';
            hash ^= zobristCastling[3];
        }
    }else if(move.targetPos == 63){
        if(representation->castle[2] != '.'){
            representation->castle[2] = '.';
            hash ^= zobristCastling[2];
        }
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

    // in black turns it becomes one xor, in white turn it becomes 2 xors which is 0
    hash ^= zobristTurn;
    representation->blockers = (representation->friends | representation->enemies);

    int tmpKing = representation->myKing;
    representation->myKing = representation->oppKing;
    representation->oppKing = tmpKing;

    unsigned long long int friends = representation->friends;
    representation->friends = representation->enemies;
    representation->enemies = friends;

    return hash;
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


MoveVal ChessAi::search(posRepresent *representation){
    int depth = 5;
    double alpha = -numeric_limits<double>::infinity();
    double beta = numeric_limits<double>::infinity();

    vector<Move> optionalMoves = this->logic->getOptionalMoves(representation, false);
    this->evaluator->reorderMoves(representation, optionalMoves);

    // Initialize Zobrist hash
    unsigned long long int hash = zobrist->computeZobristHash(*representation);
    // Track initial position
    zobrist->recordPosition(hash);
    unsigned long long int updatedHash;
    unsigned long long int newPositionHash;

    MoveVal moveVal;
    MoveVal ret;
    moveVal.value = -1 * numeric_limits<double>::infinity();
    moveVal.depth = depth;

    posRepresent sp;
    save(representation, sp);

    for (Move move : optionalMoves){
        // Update Zobrist hash for the new position
        updatedHash = update(representation, move, hash);
        zobrist->recordPosition(updatedHash);

        // double val;
        // Check for threefold repetition
        if (zobrist->isThreefoldRepetition(updatedHash) || representation->halfMove >= 100){
            ret.value = 0;
            ret.depth = depth;
        }else if(this->zobrist->transpositionExists(updatedHash) && this->zobrist->transpositionDepth(updatedHash) >= depth){
            ret = this->zobrist->transpositionEval(updatedHash);
        }else{
            ret = negaMax(representation, depth - 1, alpha, beta, updatedHash);
            ret.value = -ret.value;
            this->zobrist->saveTranspositionTable(updatedHash, ret.value, depth);
        }
        restore(representation, sp, move);

        zobrist->undoPosition(updatedHash);
        
        // i want to find a better move, if i found two moves that has the same value, 
        // if the value is inf means winning means i want to choose the move that has faster mate
        // if the value is -inf means losing means i want to choose the move that has slowest mate
        if (ret.value > moveVal.value || 
            (ret.value == moveVal.value && 
                ((ret.value == numeric_limits<double>::infinity() && ret.depth > moveVal.depth) || 
                    (ret.value == -numeric_limits<double>::infinity() && ret.depth < moveVal.depth)))) {
                        moveVal.value = ret.value;
                        moveVal.move = move;
                        moveVal.depth = ret.depth;
                        newPositionHash = updatedHash;
        }

    }
    // need to record the move!
    zobrist->recordPosition(newPositionHash); 

    // recording corrent position
    this->zobrist->saveTranspositionTable(hash, moveVal.value, depth);
    cout << "move evaluation: " << moveVal.value << endl;
    cout << "evaluator evaluation: " << this->evaluator->evaluate(&sp, optionalMoves) << endl;
    return moveVal;
}



MoveVal ChessAi::negaMax(posRepresent *representation, unsigned int depth, double alpha, double beta, unsigned long long int hash){
    vector<Move> optionalMoves = this->logic->getOptionalMoves(representation, false);
    // double value = -1 * numeric_limits<double>::infinity();

    MoveVal moveval;
    MoveVal ret;
    moveval.value = -1 * numeric_limits<double>::infinity();
    moveval.depth = depth;

    if(optionalMoves.size() == 0){
        if(!this->logic->legalPosition(representation)){
            return moveval;
        }
        moveval.value = 0;
        return moveval;
    }

    if (depth == 0){
        moveval.value = this->evaluator->evaluate(representation, optionalMoves);
        return moveval;
    }

    this->evaluator->reorderMoves(representation, optionalMoves);

    posRepresent sp;
    save(representation, sp);

    unsigned long long int updatedHash;

    for(Move move: optionalMoves){        
        // double val;
        updatedHash = update(representation, move, hash);
        zobrist->recordPosition(updatedHash);
        if (this->zobrist->isThreefoldRepetition(updatedHash) || representation->halfMove >= 100){
            // draw
            ret.value = 0;
            ret.depth = depth;
        }else if(this->zobrist->transpositionExists(updatedHash) && this->zobrist->transpositionDepth(updatedHash) >= depth){
            ret = this->zobrist->transpositionEval(updatedHash);
        }else{   
            ret = negaMax(representation, depth - 1, -beta, -alpha, updatedHash);
            ret.value = -ret.value;
            this->zobrist->saveTranspositionTable(updatedHash, ret.value, depth);
            
        }
        restore(representation, sp, move);
        zobrist->undoPosition(updatedHash);


        if (ret.value > moveval.value || 
                (ret.value == moveval.value && 
                     ((ret.value == numeric_limits<double>::infinity() && ret.depth > moveval.depth) || 
                    (ret.value == -numeric_limits<double>::infinity() && ret.depth < moveval.depth)))) {
                        moveval.value = ret.value;
                        moveval.depth = ret.depth;
        }

        // value = max(value, ret.value);
        alpha = max(alpha, moveval.value);

        if(alpha >= beta){
            break;
        }
    }
    // moveval.value = value;
    return moveval;
}

double ChessAi::quiescence(posRepresent *representation, unsigned int depth, double alpha, double beta, unsigned long long int hash)
{
    return 0.0;
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
    // std::cout << "searched:" <<this->counter << std::endl;
    // delete representation->board;
    // delete representation->castle;
    delete representation;

    return moveString;
}