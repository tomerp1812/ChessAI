#ifndef FEN_H
#define FEN_H

#include <string>

struct posRepresent
{
    unsigned long long int blockers;
    unsigned long long int friends;
    unsigned long long int enemies;
    int board[64];
    int myKing;
    int oppKing;
    int enPassant;
    int halfMove;
    int fullMove;
    char castle[4];
    char turn; // w is white, b is black
};


class PosRepresentations{
    private:

    public:
        PosRepresentations();
        posRepresent* fenToBoard(std::string fenNotation);
};


#endif