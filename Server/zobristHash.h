#ifndef ZOBRISTHASH
#define ZOBRISTHASH

#include <unordered_map>

struct posRepresent;
struct Move;

struct TranspositionEntry{
    double evaluation;
    unsigned int depth;  
};


class ZobristHash
{
private:
    const int MAX_SIZE = 8000000; // 8M entries * 16 for each entry = 128MB
    std::unordered_map<unsigned long long int, int> positionCount;
    std::unordered_map<unsigned long long int, TranspositionEntry> transpositionTable;
    std::unordered_map<char, int> castleToInt;

public:
    ZobristHash();
    ~ZobristHash();
    unsigned long long int computeZobristHash(const posRepresent& representation);
    unsigned long long int updateZobristHash(unsigned long long hash, const posRepresent& representation, const Move& move);
    void recordPosition(unsigned long long hash);
    bool isThreefoldRepetition(unsigned long long hash);
    void undoPosition(unsigned long long hash);
    void saveTranspositionTable(unsigned long long int hash, double evaluation, unsigned int depth);
    bool transpositionExists(unsigned long long int hash);
    bool transpositionExistsSameDepth(unsigned long long int hash, unsigned int depth);
    double transpositionEval(unsigned long long int hash);
};

#endif