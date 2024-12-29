#ifndef LOGIC_H
#define LOGIC_H

#include <vector>
#include "PosRepresentations.h"

struct Move
{
    int startPos;
    int targetPos;
    int promotedPiece = 0;
    double guessedValue = 0.0; // for reordering moves
};

class Logic
{
private:
    static int directions[8];
    static int knightMoves[8];
    static int numOfSquaresToEdge[64][8];
    static int knightMovesToEdge[64][8];
    static int kingMovesToEdge[64][8];
    int promotionPieces[4];
    void (Logic::*optionalMovesArr[6])(std::vector<Move>& moves, int start, posRepresent *posRep, bool onlyCaptures);
    bool (Logic::*attacksArr[6])(int start, int target, int piece);

public:
    Logic();
    ~Logic();
    void precomputKingMoveData();
    void precomputKnightMoveData();
    void precomputMoveData();
    bool legalPosition(posRepresent *posRep);
    std::vector<Move> getOptionalMoves(posRepresent* posRep, bool onlyCaptures);
    bool bishopAttack(int start, int target, int piece);
    bool rookAttack(int start, int target, int piece);
    bool knightAttack(int start, int target, int piece);
    bool queenAttack(int start, int target, int piece);
    bool kingAttack(int start, int target, int piece);
    bool pawnAttack(int start, int target, int piece);
    void bishopOptionalMoves(std::vector<Move>& moves, int start, posRepresent *posRep, bool onlyCaptures);
    void rookOptionalMoves(std::vector<Move>& moves, int start, posRepresent *posRep, bool onlyCaptures);
    void knightOptionalMoves(std::vector<Move>& moves, int start, posRepresent *posRep, bool onlyCaptures);
    void queenOptionalMoves(std::vector<Move>& moves, int start, posRepresent *posRep, bool onlyCaptures);
    void kingOptionalMoves(std::vector<Move>& moves, int start, posRepresent *posRep, bool onlyCaptures);
    void pawnOptionalMoves(std::vector<Move>& moves, int start, posRepresent *posRep, bool onlyCaptures);
    bool checkMove(int start, int targetSquare, posRepresent *posRep);
    void addMove(std::vector<Move>& moves, int start, int targetSquare);
    void addMove(std::vector<Move> &moves, int start, int targetSquare, int coefficient);
    bool checkCastling(char castle, char checkCastle, posRepresent *posRep, unsigned long long int castlingBlockers, int castleSquares[2], int start);
};

#endif