#include "ChessAi.h"
#include <limits>
#include <iostream>
#include <string>
#include <vector>
#include "Logic.h"
#include "PosRepresentations.h"
#include "evaluator.h"

using namespace std;

ChessAi::ChessAi(PosRepresentations& posRep, Evaluator& evaluator)
{
    this->posRep = &posRep;
    this->evaluator = &evaluator;
    this->counter = 0;
}

ChessAi::~ChessAi()
{
    // implementation
}

bool ChessAi::terminate(std::vector<Move> optionalMoves, int depth)
{
    if (depth <= 0 || optionalMoves.size() == 0)
    {
        return true;
    }
    return false;
}

void ChessAi::save(posRepresent *representation, Move move, savePosition& sp)
{
    sp.move = move;
    sp.piece = representation->board[move.startPos];
    sp.otherPiece = 0;
    sp.otherPiecePosition = -1;
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

    // save captured piece
    if (representation->board[move.targetPos] != 0)
    {
        sp.otherPiece = representation->board[move.targetPos];
        sp.otherPiecePosition = move.targetPos;
        
    }
    // castling
    else if (abs(sp.piece) == 6 && abs((move.targetPos - move.startPos)) == 2)
    {
        if (sp.piece > 0)
        {
            // white rook
            sp.otherPiece = 4;
        }
        else
        {
            // black rook
            sp.otherPiece = -4;
        }
        // king side castling
        if (move.targetPos > move.startPos)
        {
            sp.otherPiecePosition = move.targetPos + 1;
            
        }
        // queen side castling
        else
        {
            sp.otherPiecePosition = move.targetPos - 2;
        }
        
    }
    // en-Passant
    else if (abs(sp.piece) == 1)
    {
        // this is an en-passant capture
        if (move.targetPos == representation->enPassant)
        {
            if (sp.piece > 0)
            {
                // white pawn captures black pawn
                sp.otherPiece = -1;
                sp.otherPiecePosition = representation->enPassant - 8;
            }
            else
            {
                // black pawn captures white pawn
                sp.otherPiece = 1;
                sp.otherPiecePosition = representation->enPassant + 8;
            }
        }
    }
}

void ChessAi::update(posRepresent *representation, Move move)
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

void ChessAi::restore(posRepresent *representation, savePosition& sp){
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

    representation->board[sp.move.startPos] = sp.piece;
    representation->board[sp.move.targetPos] = 0;
    if(sp.otherPiecePosition != -1){
        representation->board[sp.otherPiecePosition] = sp.otherPiece;
    }
}

MoveVal ChessAi::search(posRepresent *representation)
{
    double alpha = -1 * numeric_limits<double>::infinity();
    double beta = numeric_limits<double>::infinity();
    int depth = 3;
    Logic *logic = new Logic();
    vector<Move> optionalMoves = logic->getOptionalMoves(representation);
    cout << optionalMoves.size() << endl;
    MoveVal moveVal;
    moveVal.value = -1 * numeric_limits<double>::infinity();
    savePosition sp;
    for (Move move : optionalMoves)
    {
        this->counter++;
        save(representation, move, sp);
        update(representation, move);
        double val = minValue(representation, alpha, beta, depth);
        restore(representation, sp);
        if (moveVal.value < val)
        {
            moveVal.move = move;
            moveVal.value = val;
        }
        if (alpha < moveVal.value)
        {
            alpha = moveVal.value;
        }
    }

    delete logic;
    return moveVal;
}

double ChessAi::maxValue(posRepresent *representation, double alpha, double beta, unsigned int depth)
{
    double value = -1 * numeric_limits<double>::infinity();
    Logic *logic = new Logic();
    vector<Move> optionalMoves = logic->getOptionalMoves(representation);

    if (terminate(optionalMoves, depth))
    {
        return this->evaluator->evaluate(representation, optionalMoves);
    }

    savePosition sp;
    for (Move move : optionalMoves)
    {
        this->counter++;
        save(representation, move, sp);
        update(representation, move);
        double val = minValue(representation, alpha, beta, depth - 1);
        restore(representation, sp);
        value = max(val, value);
        if (value >= beta)
        {
            delete logic;
            return value;
        }
        alpha = max(alpha, value);
    }

    delete logic;
    return value;
}

double ChessAi::minValue(posRepresent *representation, double alpha, double beta, unsigned int depth)
{
    double value = numeric_limits<double>::infinity();
    Logic *logic = new Logic();
    vector<Move> optionalMoves = logic->getOptionalMoves(representation);

    if (terminate(optionalMoves, depth))
    {
        return this->evaluator->evaluate(representation, optionalMoves);
    }
    savePosition sp;

    for (Move move : optionalMoves)
    {
        this->counter++;
        save(representation, move, sp);
        update(representation, move);
        double val = maxValue(representation, alpha, beta, depth - 1);
        restore(representation, sp);
        value = min(val, value);
        if (value <= alpha)
        {
            delete logic;
            return value;
        }
        beta = min(beta, value);
    }

    delete logic;

    return value;
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
    Move bestMove = search(representation).move;
    std::string moveString = moveToString(bestMove);
    std::cout << this->counter << std::endl;
    delete representation->board;
    delete representation->castle;
    delete representation;

    return moveString;
}