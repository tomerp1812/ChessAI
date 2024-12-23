#ifndef FEN_H
#define FEN_H

#include <string>

struct posRepresent
{
    int *board;
    int myKing;
    int oppKing;
    unsigned long long int blockers;
    unsigned long long int friends;
    unsigned long long int enemies;
    char turn; // w is white, b is black
    char *castle;
    int enPassant;
    int halfMove;
    int fullMove;
};


class PosRepresentations{
    private:

    public:
        PosRepresentations();
        posRepresent* fenToBoard(std::string fenNotation);
};


#endif