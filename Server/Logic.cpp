#include "Logic.h"

#include "magic-bits/include/magic_bits.hpp"
#include <stdlib.h>
#include <iostream>

#include "lookup.h"

namespace
{
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
    this->promotionPieces[0] = 2; // knight
    this->promotionPieces[1] = 3; // bishop
    this->promotionPieces[2] = 4; // rook
    this->promotionPieces[3] = 5; // queen
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
    // precomputMoveData();
    // precomputKnightMoveData();
    // precomputKingMoveData();
}

void Logic::precomputKingMoveData()
{
    for (int square = 0; square < 64; ++square)
    {
        for (int move = 0; move < 8; ++move)
        {
            this->kingMovesToEdge[square][move] = -1;
        }
    }

    for (int square = 0; square < 64; ++square)
    {
        int row = square / 8;
        int col = square % 8;
        int validMoveCount = 0;
        for (int directIndex = 0; directIndex < 8; ++directIndex)
        {
            int rowChange = this->directions[directIndex] / 8;
            int colChange = this->directions[directIndex] % 8;
            int newRow = row + rowChange;
            int newCol = col + colChange;

            if (newRow >= 0 && newRow <= 7 && newCol >= 0 && newCol <= 7)
            {
                int targetSquare = (newRow * 8) + newCol;
                this->kingMovesToEdge[square][validMoveCount++] = targetSquare;
            }
        }
    }
}

void Logic::precomputKnightMoveData()
{
    this->knightMoves[0] = 17;
    this->knightMoves[1] = 15;
    this->knightMoves[2] = 10;
    this->knightMoves[3] = 6;
    this->knightMoves[4] = -6;
    this->knightMoves[5] = -10;
    this->knightMoves[6] = -15;
    this->knightMoves[7] = -17;
    // all moves initialized to -1
    for (int square = 0; square < 64; ++square)
    {
        for (int move = 0; move < 8; ++move)
        {
            this->knightMovesToEdge[square][move] = -1;
        }
    }

    for (int square = 0; square < 64; ++square)
    {
        int row = square / 8;
        int col = square % 8;

        int validMoveCount = 0;

        for (int moveIndex = 0; moveIndex < 8; ++moveIndex)
        {
            int destSquare = square + this->knightMoves[moveIndex];
            int destRow = destSquare / 8;
            int destCol = destSquare % 8;

            if (destSquare >= 0 && destSquare < 64 &&
                ((abs(destRow - row) == 2 && abs(destCol - col) == 1) ||
                 (abs(destRow - row) == 1 && abs(destCol - col) == 2)))
            {
                this->knightMovesToEdge[square][validMoveCount++] = destSquare;
            }
        }
    }
}

void Logic::precomputMoveData()
{
    this->directions[0] = 8;
    this->directions[1] = -8;
    this->directions[2] = -1;
    this->directions[3] = 1;
    this->directions[4] = 7;
    this->directions[5] = -7;
    this->directions[6] = 9;
    this->directions[7] = -9;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
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

bool Logic::legalPosition(posRepresent *posRep)
{
    int kingIndex = posRep->myKing;
    unsigned long long int allAttacks = attacks.Queen(posRep->blockers, kingIndex);
    allAttacks |= NAttacks[kingIndex];
    // remove all friendly pieces
    allAttacks &= ~posRep->friends;
    // remove all empty squares
    allAttacks &= posRep->enemies;

    while (allAttacks)
    {
        int targetSquare = __builtin_ctzll(allAttacks);

        allAttacks &= ~(1ULL << targetSquare);

        int piece = posRep->board[targetSquare];
        if ((this->*attacksArr[abs(piece) - 1])(targetSquare, kingIndex, piece))
        {
            return false;
        }
    }
    return true;
}

// function to check if a player in check
bool Logic::check(posRepresent *posRep)
{
    unsigned long long int checkers = this->potentialCheckers;
    int kingIndex = posRep->myKing;
    unsigned long long int allAttacks = attacks.Queen(posRep->blockers, kingIndex);
    allAttacks |= NAttacks[kingIndex];
    // remove all friendly pieces
    allAttacks &= ~posRep->friends;
    // remove all empty squares
    allAttacks &= posRep->enemies;
    allAttacks &= checkers;
    // if still in check one of the checkers will still be there, therefore allAttacks will be none zero 
    return allAttacks != 0;
}

bool Logic::optionalMovesExists(posRepresent *posRep)
{
    return false;
}

// direct attacks!
bool Logic::pawnAttack(int start, int target, int piece)
{
    if (piece < 0)
    {
        return BPAttacks[start] & (1ULL << target);
    }
    return WPAttacks[start] & (1ULL << target);
}

bool Logic::knightAttack(int start, int target, int piece)
{
    if (NAttacks[start] & (1ULL << target))
    {
        return true;
    }
    return false;
}

// direct attacks!
bool Logic::bishopAttack(int start, int target, int piece)
{
    if (BAttacks[start] & (1ULL << target))
    {
        return true;
    }
    return false;
}

// direct attacks!
bool Logic::rookAttack(int start, int target, int piece)
{
    if (RAttacks[start] & (1ULL << target))
    {
        return true;
    }
    return false;
}

// direct attacks!
bool Logic::queenAttack(int start, int target, int piece)
{
    if (QAttacks[start] & (1ULL << target))
    {
        return true;
    }
    return false;
}

// direct attacks!
bool Logic::kingAttack(int start, int target, int piece)
{
    if (KAttacks[start] & (1ULL << target))
    {
        return true;
    }
    return false;
}

bool Logic::checkMove(int start, int targetSquare, posRepresent *posRep)
{
    // save current state
    int savePiece = posRep->board[start];
    int saveTarget = posRep->board[targetSquare];
    unsigned long long int friends = posRep->friends;
    unsigned long long int enemies = posRep->enemies;

    // update state
    posRep->board[targetSquare] = savePiece;
    posRep->board[start] = 0;

    posRep->friends &= ~(1ULL << start);
    posRep->friends |= (1ULL << targetSquare);

    // Remove captured enemy
    posRep->enemies &= ~(1ULL << targetSquare);

    posRep->blockers = (posRep->friends | posRep->enemies);

    bool isLegal;
    // if king moved i cannot use the pottential attackers
    if(abs(savePiece) == 6){
        isLegal = legalPosition(posRep);
    }else{
        // checks for any other piece that moves if there are still attackers more efficient
        isLegal = !check(posRep); 
    }
    

    posRep->board[targetSquare] = saveTarget;
    posRep->board[start] = savePiece;
    posRep->friends = friends;
    posRep->enemies = enemies;
    posRep->blockers = (friends | enemies);

    return isLegal;
}

void Logic::addMove(std::vector<Move> &moves, int start, int targetSquare)
{
    Move move;
    move.startPos = start;
    move.targetPos = targetSquare;
    moves.push_back(move);
}

void Logic::addMove(std::vector<Move> &moves, int start, int targetSquare, int coefficient)
{
    for (int i = 0; i < 4; i++)
    {
        Move move;
        move.startPos = start;
        move.targetPos = targetSquare;
        move.promotedPiece = coefficient * this->promotionPieces[i];
        moves.push_back(move);
    }
}

void Logic::pawnOptionalMoves(std::vector<Move> &moves, int start, posRepresent *posRep, bool onlyCaptures)
{
    unsigned long long int pawnMoves;
    if (posRep->turn == 'w')
    {
        pawnMoves = WPAttacks[start];
        // checks if en-Passant is possible for the current pawn
        if (posRep->enPassant != -1 && (pawnMoves & (1ULL << posRep->enPassant)))
        {
            // remove enemy pawn
            int targetPosition = posRep->enPassant - 8;
            int target = -1;

            posRep->enemies &= ~(1ULL << targetPosition);
            posRep->board[targetPosition] = 0;
            posRep->blockers = (posRep->friends | posRep->enemies);
            if (this->potentialCheckers == 0 || checkMove(start, posRep->enPassant, posRep))
            {
                // add en-Passant
                addMove(moves, start, posRep->enPassant);
            }
            // restore enemy pawn
            posRep->enemies |= (1ULL << targetPosition);
            posRep->board[targetPosition] = target;
            posRep->blockers = (posRep->friends | posRep->enemies);
        }

        // if there are no enemies on the attacks there is nothing to capture.
        // if there already been en-passant that means that for that square it is empty
        // so it will remove that square here.
        pawnMoves &= posRep->enemies;

        if (!(posRep->blockers & (1ULL << (start + 8))))
        {
            pawnMoves |= (1ULL << (start + 8));

            // starting position can move two squares
            if (start < 16)
            {
                if (!(posRep->blockers & (1ULL << (start + 16))))
                {
                    pawnMoves |= (1ULL << (start + 16));
                }
            }
        }
    }
    else
    {
        pawnMoves = BPAttacks[start];

        // checks if en-Passant is possible for the current pawn
        if (posRep->enPassant != -1 && (pawnMoves & (1ULL << posRep->enPassant)))
        {
            // remove enemy pawn
            int targetPosition = posRep->enPassant + 8;
            int target = 1;

            posRep->enemies &= ~(1ULL << targetPosition);
            posRep->board[targetPosition] = 0;
            posRep->blockers = (posRep->friends | posRep->enemies);
            if (this->potentialCheckers == 0 || checkMove(start, posRep->enPassant, posRep))
            {
                // add en-Passant
                addMove(moves, start, posRep->enPassant);
            }
            // restore enemy pawn
            posRep->enemies |= (1ULL << targetPosition);
            posRep->board[targetPosition] = target;
            posRep->blockers = (posRep->friends | posRep->enemies);
        }

        pawnMoves &= posRep->enemies;

        if (!(posRep->blockers & (1ULL << (start - 8))))
        {
            pawnMoves |= (1ULL << (start - 8));

            if (start > 47)
            {
                if (!(posRep->blockers & (1ULL << (start - 16))))
                {
                    pawnMoves |= (1ULL << (start - 16));
                }
            }
        }
    }

    while (pawnMoves)
    {
        int targetSquare = __builtin_ctzll(pawnMoves);

        pawnMoves &= ~(1ULL << targetSquare);

        if (this->potentialCheckers == 0 || checkMove(start, targetSquare, posRep))
        {
            // white pawn promotion
            if (posRep->turn == 'w' && targetSquare > 55)
            {
                addMove(moves, start, targetSquare, 1);
            }
            // black pawn promotion
            else if (posRep->turn == 'b' && targetSquare < 8)
            {
                addMove(moves, start, targetSquare, -1);
            }
            else
            {
                if (!onlyCaptures || posRep->board[targetSquare] != 0)
                {
                    addMove(moves, start, targetSquare);
                }
            }
        }
    }
}

void Logic::knightOptionalMoves(std::vector<Move> &moves, int start, posRepresent *posRep, bool onlyCaptures)
{
    unsigned long long int knightAttacks = NAttacks[start];
    knightAttacks &= ~posRep->friends;
    while (knightAttacks)
    {
        int targetSquare = __builtin_ctzll(knightAttacks);

        knightAttacks &= ~(1ULL << targetSquare);

        if (!onlyCaptures || posRep->board[targetSquare] != 0)
        {
            if (this->potentialCheckers == 0 || checkMove(start, targetSquare, posRep))
            {
                addMove(moves, start, targetSquare);
            }
        }
    }
}

void Logic::bishopOptionalMoves(std::vector<Move> &moves, int start, posRepresent *posRep, bool onlyCaptures)
{
    unsigned long long int bishopAttacks = attacks.Bishop(posRep->blockers, start);
    bishopAttacks &= ~posRep->friends;
    while (bishopAttacks)
    {
        // Extract the least significant bit (target square)
        int targetSquare = __builtin_ctzll(bishopAttacks);

        // Remove the extracted square from bishopAttacks
        bishopAttacks &= ~(1ULL << targetSquare);
        if (!onlyCaptures || posRep->board[targetSquare] != 0)
        {
            if (this->potentialCheckers == 0 || checkMove(start, targetSquare, posRep))
            {
                addMove(moves, start, targetSquare);
            }
        }
    }
}

void Logic::rookOptionalMoves(std::vector<Move> &moves, int start, posRepresent *posRep, bool onlyCaptures)
{
    unsigned long long int rookAttacks = attacks.Rook(posRep->blockers, start);
    rookAttacks &= ~posRep->friends;
    while (rookAttacks)
    {
        // Extract the least significant bit (target square)
        int targetSquare = __builtin_ctzll(rookAttacks);

        // Remove the extracted square from bishopAttacks
        rookAttacks &= ~(1ULL << targetSquare);
        if (!onlyCaptures || posRep->board[targetSquare] != 0)
        {
            if (this->potentialCheckers == 0 || checkMove(start, targetSquare, posRep))
            {
                addMove(moves, start, targetSquare);
            }
        }
    }
}

void Logic::queenOptionalMoves(std::vector<Move> &moves, int start, posRepresent *posRep, bool onlyCaptures)
{
    unsigned long long int queenAttacks = attacks.Queen(posRep->blockers, start);
    queenAttacks &= ~posRep->friends;

    while (queenAttacks)
    {
        int targetSquare = __builtin_ctzll(queenAttacks);

        queenAttacks &= ~(1ULL << targetSquare);

        if (!onlyCaptures || posRep->board[targetSquare] != 0)
        {
            if (this->potentialCheckers == 0 || checkMove(start, targetSquare, posRep))
            {
                addMove(moves, start, targetSquare);
            }
        }
    }
}

void Logic::kingOptionalMoves(std::vector<Move> &moves, int start, posRepresent *posRep, bool onlyCaptures)
{
    unsigned long long int kingAttacks = KAttacks[start];
    kingAttacks &= ~posRep->friends;

    while (kingAttacks)
    {
        int targetSquare = __builtin_ctzll(kingAttacks);

        kingAttacks &= ~(1ULL << targetSquare);
        posRep->myKing = targetSquare;
        if (!onlyCaptures || posRep->board[targetSquare] != 0)
        {
            if (checkMove(start, targetSquare, posRep))
            {
                addMove(moves, start, targetSquare);
            }
        }
        posRep->myKing = start;
    }

    // Castling

    // checks if not in check
    if (!onlyCaptures && legalPosition(posRep))
    {
        if (posRep->turn == 'w')
        {
            int castleSquares[2] = {5, 6};
            if (checkCastling(posRep->castle[0], 'K', posRep, 96ULL, castleSquares, start))
            {
                addMove(moves, start, 6);
            }
            castleSquares[0] = 2;
            castleSquares[1] = 3;
            if (checkCastling(posRep->castle[1], 'Q', posRep, 14ULL, castleSquares, start))
            {
                addMove(moves, start, 2);
            }
        }
        else
        {
            int castleSquares[2] = {61, 62};
            if (checkCastling(posRep->castle[2], 'k', posRep, (3ULL << 61), castleSquares, start))
            {
                addMove(moves, start, 62);
            }
            castleSquares[0] = 58;
            castleSquares[1] = 59;
            if (checkCastling(posRep->castle[3], 'q', posRep, (7ULL << 57), castleSquares, start))
            {
                addMove(moves, start, 58);
            }
        }
    }
}

bool Logic::checkCastling(char castle, char checkCastle, posRepresent *posRep, unsigned long long int castlingBlockers, int castleSquares[2], int start)
{
    // check if in the FEN notation there is the right character, if so, means rook and king for this side never moved yet.
    if (castle == checkCastle)
    {
        // checks if no pieces are standing between the king and the rook
        if (!(posRep->blockers & castlingBlockers))
        {
            int counter = 0;
            for (int i = 0; i < 2; ++i)
            {
                int square = castleSquares[i];
                posRep->myKing = square;
                if (checkMove(start, square, posRep))
                {
                    counter++;
                }
                posRep->myKing = start;
            }
            if (counter == 2)
            {
                return true;
            }
        }
    }
    return false;
}

std::vector<Move> Logic::getOptionalMoves(posRepresent *posRep, bool onlyCaptures)
{
    std::vector<Move> moves;
    unsigned long long int friends = posRep->friends;
    getPotentialCheckers(posRep);

    while (friends)
    {
        int start = __builtin_ctzll(friends);
        int piece = posRep->board[start];
        (this->*optionalMovesArr[abs(piece) - 1])(moves, start, posRep, onlyCaptures);
        friends &= ~(1ULL << start);
    }
    return moves;
}

void Logic::getPotentialCheckers(posRepresent *posRep)
{
    this->potentialCheckers = 0;
    int kingIndex = posRep->myKing;

    // get all pieces that in line of sight of the king or potential ones
    unsigned long long int allAttacks = attacks.Queen(posRep->enemies, kingIndex);
    allAttacks |= NAttacks[kingIndex];
    allAttacks &= posRep->enemies;

    while(allAttacks){
        int targetSquare = __builtin_ctzll(allAttacks);
        int piece = posRep->board[targetSquare];

        if ((this->*attacksArr[abs(piece) - 1])(targetSquare, kingIndex, piece))
        {
            this->potentialCheckers |= (1ULL << targetSquare);
        }


        allAttacks &= ~(1ULL << targetSquare);
    }
}
