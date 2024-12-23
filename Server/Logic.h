#ifndef LOGIC_H
#define LOGIC_H

#include <vector>
#include "PosRepresentations.h"

struct Move
{
    int startPos;
    int targetPos;
};

class Logic
{
private:
    static int directions[8];
    static int knightMoves[8];
    static int numOfSquaresToEdge[64][8];
    static int knightMovesToEdge[64][8];
    static int kingMovesToEdge[64][8];
    void (Logic::*optionalMovesArr[6])(std::vector<Move>& moves, int start, posRepresent *posRep);
    bool (Logic::*attacksArr[6])(int start, int target, int piece);

public:
    Logic();
    ~Logic();
    void precomputKingMoveData();
    void precomputKnightMoveData();
    void precomputMoveData();
    bool legalPosition(posRepresent *posRep);
    std::vector<Move> getOptionalMoves(posRepresent* posRep);
    bool bishopAttack(int start, int target, int piece);
    bool rookAttack(int start, int target, int piece);
    bool knightAttack(int start, int target, int piece);
    bool queenAttack(int start, int target, int piece);
    bool kingAttack(int start, int target, int piece);
    bool pawnAttack(int start, int target, int piece);
    void bishopOptionalMoves(std::vector<Move>& moves, int start, posRepresent *posRep);
    void rookOptionalMoves(std::vector<Move>& moves, int start, posRepresent *posRep);
    void knightOptionalMoves(std::vector<Move>& moves, int start, posRepresent *posRep);
    void queenOptionalMoves(std::vector<Move>& moves, int start, posRepresent *posRep);
    void kingOptionalMoves(std::vector<Move>& moves, int start, posRepresent *posRep);
    void pawnOptionalMoves(std::vector<Move>& moves, int start, posRepresent *posRep);
};

#endif