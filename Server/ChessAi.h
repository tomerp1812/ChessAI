#ifndef CHESSAI_H
#define CHESSAI_H

#include <string>
#include <vector>

class PosRepresentations;
struct posRepresent;
struct Move;

struct MoveVal
{
    double value;
    std::string move;
};



class ChessAi {
    private:
        PosRepresentations* posRep;
    public:
        ChessAi(PosRepresentations posRep);
        ~ChessAi();
        double evaluate();
        MoveVal search(posRepresent* board);
        std::string run(std::string state);
        double minValue(posRepresent* representation, double alpha, double beta, unsigned int depth);
        double maxValue(posRepresent* representation, double alpha, double beta, unsigned int depth);
        bool terminate(std::vector<Move> optionalMoves, int depth);
};

#endif