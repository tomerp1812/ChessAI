#include "Logic.h"

#include "magic-bits/include/magic_bits.hpp"
#include <stdlib.h>
#include <iostream>

#include "lookup.h"

namespace{
    const magic_bits::Attacks attacks;
}

// Define static variables
int Logic::directions[8];
int Logic::knightMoves[8];
int Logic::numOfSquaresToEdge[64][8];
int Logic::knightMovesToEdge[64][8];
int Logic::kingMovesToEdge[64][8];

Logic::Logic()
{
    this->attacksArr[0] = &Logic::pawnAttack;
    this->attacksArr[1] = &Logic::knightAttack;
    this->attacksArr[2] = &Logic::bishopAttack;
    this->attacksArr[3] = &Logic::rookAttack;
    this->attacksArr[4] = &Logic::queenAttack;
    this->attacksArr[5] = &Logic::kingAttack;
    this->optionalMovesArr[0] = &Logic::pawnOptionalMoves;
    this->optionalMovesArr[1] = &Logic::knightOptionalMoves;
    this->optionalMovesArr[2] = &Logic::bishopOptionalMoves;
    this->optionalMovesArr[3] = &Logic::rookOptionalMoves;
    this->optionalMovesArr[4] = &Logic::queenOptionalMoves;
    this->optionalMovesArr[5] = &Logic::kingOptionalMoves;
    precomputMoveData();
    precomputKnightMoveData();
    precomputKingMoveData();
}

void Logic::precomputKingMoveData(){
    for(int square = 0; square < 64; ++square){
        for(int move = 0; move < 8; ++move){
            this->kingMovesToEdge[square][move] = -1;
        }
    }

    for(int square = 0; square < 64; ++square){
        int row = square / 8;
        int col = square % 8;
        int validMoveCount = 0;
        for(int directIndex = 0; directIndex < 8; ++directIndex){
            int rowChange = this->directions[directIndex] / 8;
            int colChange = this->directions[directIndex] % 8;
            int newRow = row + rowChange;
            int newCol = col + colChange;

            if(newRow >= 0 && newRow <= 7 && newCol >= 0 && newCol <= 7){
                int targetSquare = (newRow * 8) + newCol;
                this->kingMovesToEdge[square][validMoveCount++] = targetSquare;
            }
        }
    }
}

void Logic::precomputKnightMoveData(){
    this->knightMoves[0] = 17;
    this->knightMoves[1] = 15;
    this->knightMoves[2] = 10;
    this->knightMoves[3] = 6;
    this->knightMoves[4] = -6;
    this->knightMoves[5] = -10;
    this->knightMoves[6] = -15;
    this->knightMoves[7] = -17;
    // all moves initialized to -1
    for(int square = 0; square < 64; ++square){
        for(int move = 0; move < 8; ++move){
            this->knightMovesToEdge[square][move] = -1;
        }
    }

    for(int square = 0; square < 64; ++square){
        int row = square / 8;
        int col = square % 8;

        int validMoveCount = 0;

        for(int moveIndex = 0; moveIndex < 8; ++moveIndex){
            int destSquare = square + this->knightMoves[moveIndex];
            int destRow = destSquare / 8;
            int destCol = destSquare % 8;

            if (destSquare >= 0 && destSquare < 64 &&
                ((abs(destRow - row) == 2 && abs(destCol - col) == 1) ||
                (abs(destRow - row) == 1 && abs(destCol - col) == 2))) {
                this->knightMovesToEdge[square][validMoveCount++] = destSquare;
            }
           
        }
    }
}

void Logic::precomputMoveData(){
    this->directions[0] = 8;
    this->directions[1] = -8;
    this->directions[2] = -1;
    this->directions[3] = 1;
    this->directions[4] = 7;
    this->directions[5] = -7;
    this->directions[6] = 9;
    this->directions[7] = -9;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            int north = 7 - j;
            int south = j;
            int west = i;
            int east = 7 - i;

            int index = (j * 8) + i;

            this->numOfSquaresToEdge[index][0] = north;
            this->numOfSquaresToEdge[index][1] = south;
            this->numOfSquaresToEdge[index][2] = west;
            this->numOfSquaresToEdge[index][3] = east;
            this->numOfSquaresToEdge[index][4] = std::min(north, west);
            this->numOfSquaresToEdge[index][5] = std::min(south, east);
            this->numOfSquaresToEdge[index][6] = std::min(north, east);
            this->numOfSquaresToEdge[index][7] = std::min(south, west);

        }
    }
}

Logic::~Logic()
{
}

bool Logic::legalPosition(posRepresent *posRep){
    int kingIndex = posRep->myKing;
    for(int directIndex = 0; directIndex < 8; directIndex++){
        int squaresToEdge = this->numOfSquaresToEdge[kingIndex][directIndex];
        for(int n = 0; n < squaresToEdge; n++){
            int targetSquare = kingIndex + (this->directions[directIndex] * (n + 1));
            int checkPos = (1ULL << targetSquare);
            if(posRep->friends & checkPos){
                break;
            }

            if(posRep->enemies & checkPos){
                // need to check if the piece can reach our position
                int piece = posRep->board[targetSquare];
                if((this->*attacksArr[abs(piece) - 1])(targetSquare, kingIndex, piece)){
                    return false;
                }
                break;
            }
        }
    }

    // knight check
    for(int i = 0; i < 8; i++){
        int targetSquare = this->knightMovesToEdge[kingIndex][i];
        // no more optional knight moves
        if(targetSquare == -1){
            break;
        }
        // if there is an enemy piece, and the piece is knight
        if((posRep->enemies & (1ULL << targetSquare)) && (abs(posRep->board[targetSquare]) == 2)){
            return false;
        }
    }
    return true;
}

// direct attacks!
bool Logic::pawnAttack(int start, int target, int piece){
    if(piece < 0){
        return BPAttacks[start] & (1ULL << target);
    }
    return WPAttacks[start] & (1ULL << target);

}

// if needed to check will be changed, right now only legalPosition calls it, and start and target is not a knight move.
bool Logic::knightAttack(int start, int target, int piece){
    return false;    
}

// direct attacks!
bool Logic::bishopAttack(int start, int target, int piece){
    if(BAttacks[start] & (1ULL << target)){
        return true;
    }
    return false;
}

// direct attacks!
bool Logic::rookAttack(int start, int target, int piece){
    if(RAttacks[start] & (1ULL << target)){
        return true;
    }
    return false;
}

// direct attacks!
bool Logic::queenAttack(int start, int target, int piece){
    if(QAttacks[start] & (1ULL << target)){
        return true;
    }
    return false;
}

// direct attacks!
bool Logic::kingAttack(int start, int target, int piece){
    if(KAttacks[start] & (1ULL << target)){
        return true;
    }
    return false;
}

void Logic::pawnOptionalMoves(std::vector<Move>& moves, int start, posRepresent *posRep)
{
}

void Logic::knightOptionalMoves(std::vector<Move>& moves, int start, posRepresent *posRep)
{
    for(int option = 0; option < 8; option++){
        int targetSquare = this->knightMovesToEdge[start][option];
        if(targetSquare == -1){
            break;
        }
        if((posRep->friends & (1ULL << targetSquare))){
            continue;
        }
        
        // save current state
        int savePiece = posRep->board[start];
        int saveTarget = posRep->board[targetSquare];
        unsigned long long int friends = posRep->friends;
        unsigned long long int enemies = posRep->enemies;

        //update state
        posRep->board[targetSquare] = savePiece;
        posRep->board[start] = 0;

        posRep->friends &= ~(1ULL << start);
        posRep->friends |= (1ULL << targetSquare);

        if (saveTarget != 0) {
            // Remove captured enemy
            posRep->enemies &= ~(1ULL << targetSquare); 
        }
        posRep->blockers = posRep->friends | posRep->enemies;

        if (legalPosition(posRep)){
            //add move
            Move move;
            move.startPos = start;
            move.targetPos = targetSquare;
            moves.push_back(move);
        }

        posRep->board[targetSquare] = saveTarget;
        posRep->board[start] = savePiece;
        posRep->friends = friends;
        posRep->enemies = enemies;
        posRep->blockers = friends | enemies;
        
    }
}

void Logic::bishopOptionalMoves(std::vector<Move>& moves, int start, posRepresent *posRep)
{
    unsigned long long int bishopAttacks = attacks.Bishop(posRep->blockers, start);
    bishopAttacks &= ~ posRep->friends;
    while (bishopAttacks) {
        // Extract the least significant bit (target square)
        int targetSquare = __builtin_ctzll(bishopAttacks);

        // Remove the extracted square from bishopAttacks
        bishopAttacks &= ~(1ULL << targetSquare);

        // Save current state
        int savePiece = posRep->board[start];
        int saveTarget = posRep->board[targetSquare];
        unsigned long long int friends = posRep->friends;
        unsigned long long int enemies = posRep->enemies;

        // Update state
        posRep->board[targetSquare] = savePiece;
        posRep->board[start] = 0;
        posRep->friends &= ~(1ULL << start);
        posRep->friends |= (1ULL << targetSquare);

        if (saveTarget != 0) {
            // Remove captured enemy
            posRep->enemies &= ~(1ULL << targetSquare);
        }
        posRep->blockers = posRep->friends | posRep->enemies;

        // Check legality of the position
        if (legalPosition(posRep)) {
            // Add move
            Move move;
            move.startPos = start;
            move.targetPos = targetSquare;
            moves.push_back(move);
        }

        // Revert state
        posRep->board[targetSquare] = saveTarget;
        posRep->board[start] = savePiece;
        posRep->friends = friends;
        posRep->enemies = enemies;
        posRep->blockers = friends | enemies;
    }
}

void Logic::rookOptionalMoves(std::vector<Move>& moves, int start, posRepresent *posRep)
{
    unsigned long long int rookAttacks = attacks.Rook(posRep->blockers, start);
    rookAttacks &= ~ posRep->friends;
   
}

void Logic::queenOptionalMoves(std::vector<Move>& moves, int start, posRepresent *posRep)
{
    unsigned long long int queenAttacks = attacks.Queen(posRep->blockers, start);
    queenAttacks &= ~ posRep->friends;
}

void Logic::kingOptionalMoves(std::vector<Move>& moves, int start, posRepresent *posRep)
{
    //this->kingMovesToEdge[square][validMoveCount++]
    for(int i = 0; i < 8; i++){
        if(this->kingMovesToEdge[start][i] == -1){
            break;
        }
        int targetSquare = this->kingMovesToEdge[start][i];
        // can't go on own pieces
        if(((1ULL << targetSquare) & posRep->friends)){
            continue;
        }
        
        // save current state
        int savePiece = posRep->board[start];
        int saveTarget = posRep->board[targetSquare];
        unsigned long long int friends = posRep->friends;
        unsigned long long int enemies = posRep->enemies;

        //update state
        posRep->board[targetSquare] = savePiece;
        posRep->board[start] = 0;

        posRep->myKing = targetSquare;

        posRep->friends &= ~(1ULL << start);
        posRep->friends |= (1ULL << targetSquare);

        if (saveTarget != 0) {
            // Remove captured enemy
            posRep->enemies &= ~(1ULL << targetSquare); 
        }
        posRep->blockers = posRep->friends | posRep->enemies;

        if (legalPosition(posRep)){
            //add move
            Move move;
            move.startPos = start;
            move.targetPos = targetSquare;
            moves.push_back(move);
        }

        posRep->board[targetSquare] = saveTarget;
        posRep->board[start] = savePiece;
        posRep->friends = friends;
        posRep->enemies = enemies;
        posRep->myKing = start;
        posRep->blockers = friends | enemies;
        
    }
}


std::vector<Move> Logic::getOptionalMoves(posRepresent *posRep)
{
    std::vector<Move> moves;
    unsigned long long int friends = posRep->friends;

    while(friends){
        int start = __builtin_ctzll(friends);
        int piece = posRep->board[start];
        (this->*optionalMovesArr[abs(piece) - 1])(moves, start, posRep);
        friends &= ~(1ULL << start);
    }
    // for (int start = 0; start < 64; start++)
    // {
    //     int piece = posRep->board[start];
    //     if ((posRep->turn == 'w' && piece > 0) || (posRep->turn == 'b' && piece < 0))
    //     {
    //         (this->*optionalMovesArr[abs(piece) - 1])(moves, start, posRep);
    //     }
    // }
    return moves;
}
