#ifndef FEN_H
#define FEN_H

#include <string>

class PosRepresentations{
    private:

    public:
        PosRepresentations();
        int* fenToBoard(std::string fenNotation);
};


#endif