#include "evaluator.h"
#include <iostream>
#include <algorithm>
#include <climits> 
#include "magic-bits/include/magic_bits.hpp"
#include "PosRepresentations.h"
#include "lookup.h"

namespace
{
    const magic_bits::Attacks attacks;
}

// Function declaration
bool compareByGuessValue(const Move &a, const Move &b);

Evaluator::Evaluator(){
    initPieceToVal();
    initPhaseVal();
    initEvalArr();
}


Evaluator::~Evaluator(){

}

void Evaluator::initPieceToVal(){
    this->pieceToVal[1] = 100; // white pawn
    this->pieceToVal[2] = 320; // white knight
    this->pieceToVal[3] = 330; // white bishop
    this->pieceToVal[4] = 500; // white rook
    this->pieceToVal[5] = 900; // white queen
    this->pieceToVal[6] = 10000000; // white king
    this->pieceToVal[-1] = 100;// black pawn
    this->pieceToVal[-2] = 290;// black knight
    this->pieceToVal[-3] = 300;// black bishop
    this->pieceToVal[-4] = 500;// black rook
    this->pieceToVal[-5] = 900;// black queen
    this->pieceToVal[-6] = 10000000;// black king
    this->pieceToVal[0] = 0; // empty square
}


// phase values are an estimation of whether position is midle or late game.
void Evaluator::initPhaseVal(){
    this->phaseValues[0] = 1; // pawn
    this->phaseValues[1] = 1; // knight
    this->phaseValues[2] = 1; // bishop
    this->phaseValues[3] = 2; // rook
    this->phaseValues[4] = 4; // queen
    this->phaseValues[5] = 0;
}

void Evaluator::initEvalArr(){
    this->evalArr[0] = &Evaluator::knightEval;
    this->evalArr[1] = &Evaluator::bishopEval;
    this->evalArr[2] = &Evaluator::rookEval;
    this->evalArr[3] = &Evaluator::queenEval;
    this->evalArr[4] = &Evaluator::kingEval;
}

void Evaluator::setTables(char turn){
    if(turn == 'w'){
        this->friends_mg_table = white_mg_table;
        this->enemies_mg_table = black_mg_table;
        this->friends_eg_table = white_eg_table;
        this->enemies_eg_table = black_eg_table;


        this->coefficient = 1;
        this->fppmg = white_mg_passed_pawn_table;
        this->fppeg = white_eg_passed_pawn_table;
        this->eppmg = black_mg_passed_pawn_table;
        this->eppeg = black_eg_passed_pawn_table;

        this->ffm = whiteForwardMasks;
        this->efm = blackForwardMasks;

    }else{
        this->friends_mg_table = black_mg_table;
        this->enemies_mg_table = white_mg_table;
        this->friends_eg_table = black_eg_table;
        this->enemies_eg_table = white_eg_table;
        

        this->coefficient = -1;
        this->fppmg = black_mg_passed_pawn_table;
        this->fppeg = black_eg_passed_pawn_table;
        this->eppmg = white_mg_passed_pawn_table;
        this->eppeg = white_eg_passed_pawn_table;

        this->ffm = blackForwardMasks;
        this->efm = whiteForwardMasks;

    }
}

double Evaluator::pawnEval(int middleGamePercentage, int endGamePercentage, unsigned long long int friendlyPawns, unsigned long long int enemyPawns, int coefficient, const double* mgPP, const double* egPP, const unsigned long long int* forwardMask, const unsigned long long int* backwardMask){
    double mgPVal = 0;
    double egPVal = 0;
    unsigned long long int fileA = 0x0101010101010101; 
    unsigned long long int rank0 = 0xFF;
    unsigned long long int maxValue = ULLONG_MAX;

    unsigned long long int fileMask;
    unsigned long long int fileMaskLeft;
    unsigned long long int fileMaskRight;
    unsigned long long int tripleFileMask;
    unsigned long long int forwardTripleMask;
    unsigned long long int neighbors;

    int pawnProtector1 = 7 * coefficient; 
    int pawnProtector2 = 9 * coefficient;

    unsigned long long int savedFriendlyPawns = friendlyPawns;
    
    while(friendlyPawns){
        int position = __builtin_ctzll(friendlyPawns);
        int fileIndex = position % 8;
        int rankIndex = position / 8;

        fileMask = (fileA << fileIndex);
        fileMaskLeft = (fileA << (std::max(0, fileIndex - 1)));
        fileMaskRight = (fileA << (std::min(7, fileIndex + 1)));
        tripleFileMask = (fileMask | fileMaskLeft | fileMaskRight);

        forwardTripleMask = (tripleFileMask & forwardMask[rankIndex]);
        
        // this is a check of pass pawn
        if(!(forwardTripleMask & enemyPawns)){
            mgPVal += (mgPP[position]);
            egPVal += (egPP[position]);
        }


        // double pawns
        if(fileMask & savedFriendlyPawns & ~(1ULL << position)){
            mgPVal -= 4;
            egPVal -= 8;
        }
        
        // friendly pawns that are in left or right files from the current pawn
        neighbors = ((tripleFileMask ^ fileMask) & savedFriendlyPawns);

        // isolated pawns
        if(!neighbors){
            mgPVal -= 3;
            egPVal -= 5;
        }
        // backward pawns, pawn that all his neighbors are ahead of him
        else if(!((forwardMask[rankIndex] ^ maxValue) & neighbors)){
            mgPVal -= 2;
            egPVal -= 4;
        }
        // protected white pawns
        else if(coefficient == 1){
            if(rankIndex > 1){
                if(fileIndex < 7 && ((1ULL << (position - pawnProtector1)) & savedFriendlyPawns)){
                    mgPVal += 3;
                    egPVal += 6;
                }
                if(fileIndex > 0 && ((1ULL << (position - pawnProtector2)) & savedFriendlyPawns)){
                    mgPVal += 3;
                    egPVal += 6;
                }
            }
        }
        // protected black pawns
        else{
            if(rankIndex < 7){
                if(fileIndex > 0 && ((1ULL << (position - pawnProtector1)) & savedFriendlyPawns)){
                    mgPVal += 3;
                    egPVal += 6;
                }
                if(fileIndex < 7 && ((1ULL << (position - pawnProtector2)) & savedFriendlyPawns)){
                    mgPVal += 3;
                    egPVal += 6;
                }
            }
        }

        friendlyPawns &= ~(1ULL << position);
    }

    return (mgPVal * middleGamePercentage) + (egPVal * endGamePercentage);
}

double Evaluator::knightEval(const posRepresent* representation, int middleGamePercentage, int endGamePercentage)
{
    return 0.0;
}

double Evaluator::bishopEval(const posRepresent* representation, int middleGamePercentage, int endGamePercentage){
    double val = 0;

    unsigned long long int friendlyBishops = this->fBB[2];
    unsigned long long int enemyBishops = this->eBB[2];

    // giving more points to the bishop pair
    if(__builtin_popcountll(friendlyBishops) == 2){
        val += (8 * middleGamePercentage) + (4 * endGamePercentage);
    }
    if(__builtin_popcountll(enemyBishops) == 2){
        val -= (8 * middleGamePercentage) + (4 * endGamePercentage);
    }

   
    while(friendlyBishops){
        int position = __builtin_ctzll(friendlyBishops);

         // adding points for movement, attacking and defending
        unsigned long long int allAttacks = attacks.Bishop(representation->blockers, position);
        unsigned long long int defending = (allAttacks & representation->friends);
        unsigned long long int attacking = (allAttacks & representation->enemies);
        unsigned long long int movement = (allAttacks & (~representation->blockers)); 
        val += (8 * __builtin_popcountll(attacking)) + (4 * __builtin_popcountll(defending)) + (2 * __builtin_popcountll(movement));
        friendlyBishops &= ~(1ULL << position);
    }

    while(enemyBishops){
        int position = __builtin_ctzll(enemyBishops);
        unsigned long long int allAttacks = attacks.Bishop(representation->blockers, position);
        unsigned long long int defending = (allAttacks & representation->enemies);
        unsigned long long int attacking = (allAttacks & representation->friends);
        unsigned long long int movement = (allAttacks & (~representation->blockers)); 
        val -= ((8 * __builtin_popcountll(attacking)) + (4 * __builtin_popcountll(defending)) + (2 * __builtin_popcountll(movement)));
        enemyBishops &= ~(1ULL << position);
    }
    return val;
}
double Evaluator::rookEval(const posRepresent* representation, int middleGamePercentage, int endGamePercentage){
    double val = 0;

    unsigned long long int friendlyRooks = this->fBB[3];
    unsigned long long int enemyRooks = this->eBB[3];
   
    while(friendlyRooks){
        int position = __builtin_ctzll(friendlyRooks);

         // adding points for movement, attacking and defending
        unsigned long long int allAttacks = attacks.Rook(representation->blockers, position);
        unsigned long long int defending = (allAttacks & representation->friends);
        unsigned long long int attacking = (allAttacks & representation->enemies);
        unsigned long long int movement = (allAttacks & (~representation->blockers)); 
        val += (6 * __builtin_popcountll(attacking)) + (3 * __builtin_popcountll(defending)) + __builtin_popcountll(movement);
        friendlyRooks &= ~(1ULL << position);
    }

    while(enemyRooks){
        int position = __builtin_ctzll(enemyRooks);
        unsigned long long int allAttacks = attacks.Rook(representation->blockers, position);
        unsigned long long int defending = (allAttacks & representation->enemies);
        unsigned long long int attacking = (allAttacks & representation->friends);
        unsigned long long int movement = (allAttacks & (~representation->blockers)); 
        val -= ((6 * __builtin_popcountll(attacking)) + (3 * __builtin_popcountll(defending)) + __builtin_popcountll(movement));
        enemyRooks &= ~(1ULL << position);
    }
    return val;
}

double Evaluator::queenEval(const posRepresent* representation, int middleGamePercentage, int endGamePercentage){
    double val = 0;

    unsigned long long int friendlyQueens = this->fBB[4];
    unsigned long long int enemyQueens = this->eBB[4];
   
    while(friendlyQueens){
        int position = __builtin_ctzll(friendlyQueens);

         // adding points for movement, attacking and defending
        unsigned long long int allAttacks = attacks.Queen(representation->blockers, position);
        unsigned long long int defending = (allAttacks & representation->friends);
        unsigned long long int attacking = (allAttacks & representation->enemies);
        unsigned long long int movement = (allAttacks & (~representation->blockers)); 
        val += (3 * __builtin_popcountll(attacking)) + (2 * __builtin_popcountll(defending)) + __builtin_popcountll(movement);
        friendlyQueens &= ~(1ULL << position);
    }

    while(enemyQueens){
        int position = __builtin_ctzll(enemyQueens);
        unsigned long long int allAttacks = attacks.Queen(representation->blockers, position);
        unsigned long long int defending = (allAttacks & representation->enemies);
        unsigned long long int attacking = (allAttacks & representation->friends);
        unsigned long long int movement = (allAttacks & (~representation->blockers)); 
        val -= ((3 * __builtin_popcountll(attacking)) + (2 * __builtin_popcountll(defending)) + __builtin_popcountll(movement));
        enemyQueens &= ~(1ULL << position);
    }
    return val;
}

double Evaluator::kingEval(const posRepresent* representation, int middleGamePercentage, int endGamePercentage)
{
    return 0;
}

double Evaluator::piecesEval(const posRepresent *representation, int middleGamePercentage, int endGamePercentage)
{
    double val = 0;
    
    // pawns contributions
    val += pawnEval(middleGamePercentage, endGamePercentage, this->fBB[0], this->eBB[0], coefficient, this->fppmg, this->fppeg, this->ffm, this->efm);
    val -= pawnEval(middleGamePercentage, endGamePercentage, this->eBB[0], this->fBB[0], -coefficient, this->eppmg, this->eppeg, this->efm, this->ffm);

    val += knightEval(representation, middleGamePercentage, endGamePercentage);
    val += bishopEval(representation, middleGamePercentage, endGamePercentage);
    val += rookEval(representation, middleGamePercentage, endGamePercentage);
    val += queenEval(representation, middleGamePercentage, endGamePercentage);
    val += kingEval(representation, middleGamePercentage, endGamePercentage);
    

    return val;
}

double Evaluator::evaluate(const posRepresent *representation){

    // init bitboards
    for(int i = 0; i < 6; i++){
        this->fBB[i] = 0;
        this->eBB[i] = 0;
    }


    double val = 0;
    double mgPositionsVals = 0;
    double egPositionsVals = 0;
    int totalPhase = 0;
    unsigned long long int friends = representation->friends;
    unsigned long long int enemies = representation->enemies;
    unsigned long long int blockers = representation->blockers;

    setTables(representation->turn);

    while(friends){
        // fetching the position
        int position = __builtin_ctzll(friends);
        // value of friendly piece
        val += this->pieceToVal[representation->board[position]];
        int pieceInMyDictionary = abs(representation->board[position]) - 1;
        this->fBB[pieceInMyDictionary] |= (1ULL << position);

        // if(pieceInMyDictionary > 0){
        //     val += (this->*evalArr[pieceInMyDictionary - 1])(position, representation->friends, representation->enemies);
        // }

        // calculating in which phase we are
        totalPhase += this->phaseValues[pieceInMyDictionary];

        // evaluate pieces positions
        mgPositionsVals += (*this->friends_mg_table[pieceInMyDictionary])[position];
        egPositionsVals += (*this->friends_eg_table[pieceInMyDictionary])[position];

        friends &= ~(1ULL << position);
    }

    while(enemies){
        // fetching the position
        int position = __builtin_ctzll(enemies);
        // value of enemy piece
        val -= this->pieceToVal[representation->board[position]];
        int pieceInMyDictionary = abs(representation->board[position]) - 1;
        this->eBB[pieceInMyDictionary] |= (1ULL << position);

        // if(pieceInMyDictionary > 0){
        //     val -= (this->*evalArr[pieceInMyDictionary - 1])(position, representation->enemies, representation->friends);
        // }

        // calculating in which phase we are
        totalPhase += this->phaseValues[pieceInMyDictionary];

        // evaluate pieces positions
        mgPositionsVals -= (*this->enemies_mg_table[pieceInMyDictionary])[position];
        egPositionsVals -= (*this->enemies_eg_table[pieceInMyDictionary])[position];

        enemies &= ~(1ULL << position);
    }
    
    totalPhase = std::min(totalPhase, 24);
    double middleGamePercentage = (double(totalPhase) / 24);
    double endGamePercentage = (1 - (double(totalPhase) / 24));
    val += (mgPositionsVals * middleGamePercentage) + (egPositionsVals * endGamePercentage);

    val += piecesEval(representation, middleGamePercentage, endGamePercentage);
    return val;
    
}

// not part of the Evaluator class, just a comperison funciton
bool compareByGuessValue(const Move &a, const Move &b){
    return a.guessedValue > b.guessedValue;
}

void Evaluator::reorderMoves(posRepresent *representation, std::vector<Move>& optionalMoves){


    setTables(representation->turn);

    unsigned long long int blockers = representation->blockers;
    int totalPhase = 0;

    while(blockers){
        int position = __builtin_ctzll(blockers);
        int pieceInMyDictionary = abs(representation->board[position]) - 1;

        // calculating in which phase we are
        totalPhase += this->phaseValues[pieceInMyDictionary];

        blockers &= ~(1ULL << position);
    }

    totalPhase = std::min(totalPhase, 24);

    for(Move& move: optionalMoves){
        double guessedValue = 0.0;
        // better to capture opponent pieces
        guessedValue += this->pieceToVal[representation->board[move.targetPos]];

        // promotions better be checked
        if(move.promotedPiece != 0){
            guessedValue += this->pieceToVal[move.promotedPiece] - 100;
        }

        int pieceInDict = abs(representation->board[move.startPos]) - 1;

        // prefer moving to better squares
        guessedValue += ((*this->friends_mg_table[pieceInDict])[move.targetPos] * (double(totalPhase) / 24));
        guessedValue += ((*this->friends_eg_table[pieceInDict])[move.targetPos] * (1 - (double(totalPhase) / 24)));
  
        move.guessedValue = guessedValue;
    }
    
    // reordering the moves from better moves to worse, to prune more moves
    std::sort(optionalMoves.begin(), optionalMoves.end(), compareByGuessValue);
}






