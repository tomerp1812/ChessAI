#include "zobristHash.h"
#include <limits>
#include <iostream>
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

unsigned long long int ZobristHash::updateZobristHash(unsigned long long hash, const posRepresent &representation, const Move &move)
{
    // Remove piece from start position
    int piece = representation.board[move.startPos];
    int pieceIndex = (piece > 0) ? piece - 1 : 5 - piece;
    hash ^= zobristTable[pieceIndex][move.startPos];

    // Add piece to target position
    hash ^= zobristTable[pieceIndex][move.targetPos];

    // Remove captured piece
    if (representation.board[move.targetPos] != 0)
    {
        int capturedPiece = representation.board[move.targetPos];
        int capturedIndex = (capturedPiece > 0) ? capturedPiece - 1 : 5 - capturedPiece;
        hash ^= zobristTable[capturedIndex][move.targetPos];
    }

    // pawn capture en-passant
    if (abs(piece) == 1 && move.targetPos == representation.enPassant)
    {
        int captured;
        int dist;
        if (representation.turn == 'w')
        {
            captured = 6; // position of black pawn in the zobristTable
            dist = -8;
        }
        else
        {
            captured = 0; // position of white pawn in the zobristTable
            dist = 8;
        }
        hash ^= zobristTable[captured][move.targetPos + dist];
    }

    // Update en-passant
    if (representation.enPassant >= 0)
    {
        int file = representation.enPassant % 8;
        hash ^= zobristEnPassant[file];
    }
    // Update en-passant after move
    if (abs(piece) == 1 && abs(move.startPos - move.targetPos) == 16)
    {
        int file = move.startPos % 8;
        hash ^= zobristEnPassant[file];
    }

    char castleChar;
    // Update castling rights
    for (int i = 0; i < 4; ++i)
    {
        castleChar = representation.castle[i];
        if (castleChar == '.')
        {
            hash ^= zobristCastling[this->castleToInt[castleChar]];
        }
    }

    // Update turn
    hash ^= zobristTurn;

    return hash;
}

void ZobristHash::recordPosition(unsigned long long hash)
{
    if (this->positionCount.find(hash) == this->positionCount.end())
    {
        this->positionCount[hash] = 0;
    }
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

void ZobristHash::saveTranspositionTable(unsigned long long int hash, double evaluation, unsigned int depth)
{
    // maybe add flag for how good the depth is
    if (!transpositionExists(hash) || depth > this->transpositionTable[hash].depth)
    {
        this->transpositionTable[hash] = {evaluation, depth};
    }
}

bool ZobristHash::transpositionExists(unsigned long long int hash)
{
    return this->transpositionTable.find(hash) != this->transpositionTable.end();
}

bool ZobristHash::transpositionExistsSameDepth(unsigned long long int hash, unsigned int depth)
{
    auto it = this->transpositionTable.find(hash);
    return (it != this->transpositionTable.end() && it->second.depth == depth);
}

double ZobristHash::transpositionEval(unsigned long long int hash)
{
    auto it = this->transpositionTable.find(hash);
    if (it != this->transpositionTable.end())
    {
        return it->second.evaluation;
    }
    return -std::numeric_limits<double>::infinity();
}
