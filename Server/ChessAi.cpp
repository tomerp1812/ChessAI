#include "ChessAi.h"
#include <limits>
#include <iostream>
#include <string>
#include <vector>
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
    // this->totalDepth = 8;
    // this->halfDepth = this->totalDepth / 2;
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


MoveVal ChessAi::iddfs(posRepresent *representation)
{
    this->startTime = chrono::steady_clock::now();

    // initializations
    this->newPositionHash = 0;
    vector<Move> optionalMoves = this->logic->getOptionalMoves(representation, false);
    this->evaluator->reorderMoves(representation, optionalMoves);
    // Initialize Zobrist hash
    unsigned long long int hash = zobrist->computeZobristHash(*representation);
    // Track initial position
    zobrist->recordPosition(hash);
    this->startingDepth = 8;
    this->totalDepth = 8;
    this->halfDepth = this->totalDepth / 2;

    MoveVal bestMove;
    MoveVal currentMove;

    for(;this->totalDepth < 200;){
        currentMove = search(representation, optionalMoves, hash);
        // found a mate no reason to keep searching
        if(currentMove.value == numeric_limits<double>::infinity() ||
        currentMove.value == -numeric_limits<double>::infinity()){
            bestMove = currentMove;
            break;
        }
        auto currentTime = chrono::steady_clock::now();
        auto elapsedTime = chrono::duration_cast<chrono::milliseconds>(currentTime - this->startTime).count();
        if(this->totalDepth == this->startingDepth || elapsedTime < timeLimit){
            bestMove = currentMove;
        }else{
            break;
        }
        this->totalDepth++;
        this->halfDepth = this->totalDepth / 2;
    }
    
    // need to record the move!
    zobrist->recordPosition(bestMove.positionHash); 

    // recording corrent position
    this->zobrist->saveTranspositionTable(hash, bestMove.value, this->totalDepth, bestMove.mateIn);

    // data printers
    cout << "depth: " << this->totalDepth << std::endl;
    cout << "mate in: " << bestMove.mateIn << std::endl;
    cout << "move evaluation: " << bestMove.value << endl;
    return bestMove;
}

MoveVal ChessAi::search(posRepresent *representation, const std::vector<Move>& optionalMoves, unsigned long long int hash){
    double alpha = -numeric_limits<double>::infinity();
    double beta = numeric_limits<double>::infinity();
    unsigned long long int updatedHash;

    MoveVal moveVal;
    MoveVal ret;
    moveVal.value = -1 * numeric_limits<double>::infinity();
    moveVal.depth = this->totalDepth;

    posRepresent sp;
    save(representation, sp);

    for (Move move : optionalMoves){
        // check if time passed and this is not the first itteration
        auto currentTime = chrono::steady_clock::now();
        auto elapsedTime = chrono::duration_cast<chrono::milliseconds>(currentTime - this->startTime).count();
        if(this->startingDepth < this->totalDepth && elapsedTime > timeLimit){
            return moveVal;
        }

        // Update Zobrist hash for the new position
        updatedHash = update(representation, move, hash);
        zobrist->recordPosition(updatedHash);

        // Check for threefold repetition
        if (zobrist->isThreefoldRepetition(updatedHash) || representation->halfMove >= 100){
            ret.value = 0;
            ret.depth = this->totalDepth;
        }else if(this->zobrist->transpositionExists(updatedHash) && this->zobrist->transpositionDepth(updatedHash) >= this->totalDepth){
            ret = this->zobrist->transpositionEval(updatedHash);
        }else{
            ret = negaMax(representation, this->totalDepth - 1, alpha, beta, updatedHash);
            ret.value = -ret.value;
            this->zobrist->saveTranspositionTable(updatedHash, ret.value, this->totalDepth, ret.mateIn);
        }
        restore(representation, sp, move);

        zobrist->undoPosition(updatedHash);
        
        // i want to find a better move, if i found two moves that has the same value, 
        // if the value is inf means winning means i want to choose the move that has faster mate
        // if the value is -inf means losing means i want to choose the move that has slowest mate
        if (ret.value == moveVal.value) {
            // if found mate
            if((ret.value == numeric_limits<double>::infinity() && moveVal.mateIn > ret.mateIn)
            || (ret.value == -numeric_limits<double>::infinity() && moveVal.mateIn <= ret.mateIn)){
                moveVal.mateIn = ret.mateIn + 1;
                moveVal.value = ret.value;
                moveVal.move = move;
                moveVal.depth = ret.depth;
                moveVal.positionHash = updatedHash;
            }
        }else if(ret.value > moveVal.value){
            if(ret.value == numeric_limits<double>::infinity() || ret.value == -numeric_limits<double>::infinity()){
                moveVal.mateIn = ret.mateIn + 1;
            }
            moveVal.value = ret.value;
            moveVal.depth = ret.depth;
            moveVal.move = move;
            moveVal.positionHash = updatedHash;
        }
    }
    return moveVal;
}



MoveVal ChessAi::negaMax(posRepresent *representation, unsigned int depth, double alpha, double beta, unsigned long long int hash){
    vector<Move> optionalMoves = this->logic->getOptionalMoves(representation, false);
    MoveVal moveVal;
    MoveVal ret;
    moveVal.value = -1 * numeric_limits<double>::infinity();
    moveVal.depth = depth;

    if(optionalMoves.size() == 0){
        if(!this->logic->legalPosition(representation)){
            moveVal.mateIn = 0;
            return moveVal;
        }
        moveVal.value = 0;
        return moveVal;
    }

    if (depth == this->halfDepth){
        return quiescence(representation, this->halfDepth, alpha, beta, hash);
    }

    this->evaluator->reorderMoves(representation, optionalMoves);

    posRepresent sp;
    save(representation, sp);

    unsigned long long int updatedHash;

    for(Move move: optionalMoves){        
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
            this->zobrist->saveTranspositionTable(updatedHash, ret.value, depth, ret.mateIn);
            
        }
        restore(representation, sp, move);
        zobrist->undoPosition(updatedHash);


        

        if (ret.value == moveVal.value) {
            // if found mate
            if((ret.value == numeric_limits<double>::infinity() && moveVal.mateIn > ret.mateIn)
            || (ret.value == -numeric_limits<double>::infinity() && moveVal.mateIn <= ret.mateIn)){
                moveVal.mateIn = ret.mateIn + 1;
                moveVal.value = ret.value;
                moveVal.depth = ret.depth;
            }
        }else if(ret.value > moveVal.value){
            if(ret.value == numeric_limits<double>::infinity() || ret.value == -numeric_limits<double>::infinity()){
                moveVal.mateIn = ret.mateIn + 1;
            }
            moveVal.value = ret.value;
            moveVal.depth = ret.depth;
        }

        alpha = max(alpha, moveVal.value);

        if(alpha >= beta){
            break;
        }
    }
    return moveVal;
}

MoveVal ChessAi::quiescence(posRepresent *representation, unsigned int depth, double alpha, double beta, unsigned long long int hash)
{
    MoveVal moveval;
    MoveVal ret;
    moveval.value = this->evaluator->evaluate(representation);
    moveval.depth = depth;

    if(moveval.value >= beta){
        moveval.value = beta;
        return moveval;
    }

    if(depth == 0){
        return moveval;
    }

    alpha = max(alpha, moveval.value);

    // only captures
    vector<Move> optionalMoves = this->logic->getOptionalMoves(representation, true);
    this->evaluator->reorderMoves(representation, optionalMoves);

    unsigned long long int updatedHash;
    posRepresent sp;
    save(representation, sp);

    for(Move move: optionalMoves){
        updatedHash = update(representation, move, hash);
        zobrist->recordPosition(updatedHash);

        if(this->zobrist->transpositionExists(updatedHash) && this->zobrist->transpositionDepth(updatedHash) >= depth){
            ret = this->zobrist->transpositionEval(updatedHash);
        }else{
            ret = quiescence(representation, depth - 1, -beta, -alpha, updatedHash);
            ret.value = -ret.value;
            this->zobrist->saveTranspositionTable(updatedHash, ret.value, depth, ret.mateIn);
        }

        restore(representation, sp, move);
        zobrist->undoPosition(updatedHash);


        if (ret.value >= moveval.value){
            moveval.value = ret.value;
            moveval.depth = ret.depth;
        }

        alpha = max(alpha, moveval.value);

        if(alpha >= beta){
            break;
        }
    }
    return moveval;
}

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
    Move bestMove = iddfs(representation).move;
    std::string moveString = moveToString(bestMove);
    delete representation;
    return moveString;
}