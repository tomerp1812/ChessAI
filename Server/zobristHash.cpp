#include "zobristHash.h"
#include <limits>
#include <iostream>
#include "ChessAi.h"
#include "PosRepresentations.h"
#include "lookup.h"
#include "Logic.h"

ZobristHash::ZobristHash()
{
    this->castleToInt['K'] = 0;
    this->castleToInt['Q'] = 1;
    this->castleToInt['k'] = 2;
    this->castleToInt['q'] = 3;
}

ZobristHash::~ZobristHash()
{
}

unsigned long long int ZobristHash::computeZobristHash(const posRepresent &representation)
{
    unsigned long long int hash = 0;

    // Add piece positions
    unsigned long long int blockers = representation.blockers;
    while (blockers)
    {
        int position = __builtin_ctzll(blockers);
        int piece = representation.board[position];

        if (piece != 0)
        {
            int index = (piece > 0) ? piece - 1 : 5 - piece;
            hash ^= zobristTable[index][position];
        }

        blockers &= ~(1ULL << position);
    }

    char castleChar;
    // Add castling rights
    for (int i = 0; i < 4; ++i)
    {
        castleChar = representation.castle[i];
        if (castleChar != '.')
        {
            hash ^= zobristCastling[this->castleToInt[castleChar]];
        }
    }

    // Add en-passant target
    if (representation.enPassant >= 0)
    {
        int file = representation.enPassant % 8;
        hash ^= zobristEnPassant[file];
    }

    // Add turn
    if (representation.turn == 'b')
    {
        hash ^= zobristTurn;
    }

    return hash;
}

void ZobristHash::recordPosition(unsigned long long hash)
{
    this->positionCount[hash]++;
}

bool ZobristHash::isThreefoldRepetition(unsigned long long hash)
{

    if (this->positionCount[hash] > 3)
    {
        std::cout << "problem in isThreefoldRepetition!!! > 3" << std::endl;
    }
    else if (this->positionCount[hash] < 0)
    {
        std::cout << "problem in isThreefoldRepetition!!! < 0" << std::endl;
    }
    return this->positionCount[hash] == 3;
}

void ZobristHash::undoPosition(unsigned long long hash)
{
    if (this->positionCount.find(hash) == this->positionCount.end())
    {
        std::cout << "problem in undoPosition!!! before decrement" << std::endl;
    }
    this->positionCount[hash]--;
    if (this->positionCount[hash] < 0)
    {
        std::cout << "problem in undoPosition!!!" << std::endl;
    }
}

void ZobristHash::saveTranspositionTable(unsigned long long int hash, double evaluation, unsigned int depth, int mateIn)
{
    MoveVal mv;
    mv.value = evaluation;
    mv.depth = depth;
    mv.mateIn = mateIn;
    this->transpositionTable[hash] = mv;
}


bool ZobristHash::transpositionExists(unsigned long long int hash)
{
    return this->transpositionTable.find(hash) != this->transpositionTable.end();
}

unsigned int ZobristHash::transpositionDepth(unsigned long long int hash){
    return this->transpositionTable[hash].depth;
}

MoveVal ZobristHash::transpositionEval(unsigned long long int hash)
{
    auto it = this->transpositionTable.find(hash);
    if (it != this->transpositionTable.end())
    {

        return it->second;
    }
    // shouldn't get here
    std::cout << "problem in transpositionEval!" << std::endl;
    MoveVal mv;
    mv.value = negInf;
    mv.depth = 0;
    return mv;
}
