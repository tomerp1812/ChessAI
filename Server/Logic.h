#ifndef LOGIC_H
#define LOGIC_H

#include <vector>

struct Move
{
    int startPos;
    int targetPos;
};

class Logic
{
private:
    std::vector<Move> optionalMoves;

public:
    Logic();
    ~Logic();
    std::vector<Move> getOptionalMoves();
};

#endif