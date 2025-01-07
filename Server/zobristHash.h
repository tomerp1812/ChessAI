#ifndef ZOBRISTHASH
#define ZOBRISTHASH

#include <unordered_map>

struct posRepresent;
struct Move;
struct MoveVal;

// struct TranspositionEntry{
//     double evaluation;
//     unsigned int depth;  
// };


class ZobristHash
{
private:
    const int MAX_SIZE = 8000000; // 8M entries * 16 for each entry = 128MB
    std::unordered_map<unsigned long long int, int> positionCount;
    std::unordered_map<unsigned long long int, MoveVal> transpositionTable;
    std::unordered_map<char, int> castleToInt;

public:
    ZobristHash();
    ~ZobristHash();
    unsigned long long int computeZobristHash(const posRepresent& representation);
    void recordPosition(unsigned long long hash);
    bool isThreefoldRepetition(unsigned long long hash);
    void undoPosition(unsigned long long hash);
    void saveTranspositionTable(unsigned long long int hash, double evaluation, unsigned int depth, int mateIn);
    unsigned int transpositionDepth(unsigned long long int hash);
    bool transpositionExists(unsigned long long int hash);
    MoveVal transpositionEval(unsigned long long int hash);
};

#endif