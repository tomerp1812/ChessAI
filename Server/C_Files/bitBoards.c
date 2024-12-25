#include <stdio.h>
typedef unsigned long long uint64;


uint64 rBitBoard(int square){
    uint64 ans = 0;
    int row = square / 8;
    int col = square % 8;

    for (int i = 0; i < 8; i++) {
        if (i != row) {
            ans |= (1ULL << (i * 8 + col));
        }
    }
    for(int j = 0; j < 8; j++){
        if(j != col){
            ans |= (1ULL << (row * 8 + j));
        }
    }
    
    return ans;
}

uint64 bBitBoard(int square){
    uint64 ans = 0;
    int row = square / 8;
    int col = square % 8;

    for (int i = 1; row - i >= 0 && col - i >= 0; i++) {
        ans |= (1ULL << ((row - i) * 8 + (col - i)));
    }
    for (int i = 1; row - i >= 0 && col + i < 8; i++) {
        ans |= (1ULL << ((row - i) * 8 + (col + i)));
    }
    for (int i = 1; row + i < 8 && col - i >= 0; i++) {
        ans |= (1ULL << ((row + i) * 8 + (col - i)));
    }
    for (int i = 1; row + i < 8 && col + i < 8; i++) {
        ans |= (1ULL << ((row + i) * 8 + (col + i)));
    }

    return ans;
}

uint64 kBitBoard(int square) {
    uint64 ans = 0;
    int row = square / 8;
    int col = square % 8;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int newRow = row + i;
            int newCol = col + j;
            if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
                ans |= (1ULL << (newRow * 8 + newCol));
            }
        }
    }

    return ans;
}


uint64 wPawnBitBoard(int square) {
    uint64 ans = 0;
    int row = square / 8;
    int col = square % 8;

    if (row < 7) {  // Ensure not going beyond the board
        if (col > 0) ans |= (1ULL << ((row + 1) * 8 + (col - 1)));  // Capturing left
        if (col < 7) ans |= (1ULL << ((row + 1) * 8 + (col + 1)));  // Capturing right
    }

    return ans;
}

uint64 bPawnBitBoard(int square) {
    uint64 ans = 0;
    int row = square / 8;
    int col = square % 8;

    if (row > 0) {  // Ensure not going beyond the board
        if (col > 0) ans |= (1ULL << ((row - 1) * 8 + (col - 1)));  // Capturing left
        if (col < 7) ans |= (1ULL << ((row - 1) * 8 + (col + 1)));  // Capturing right
    }

    return ans;
}

uint64 nBitBoard(int square) {
    uint64 ans = 0;
    int row = square / 8;
    int col = square % 8;

    // All possible knight moves relative to the current square
    int knightMoves[8][2] = {
        {-2, -1}, {-1, -2}, {1, -2}, {2, -1}, 
        {2, 1}, {1, 2}, {-1, 2}, {-2, 1}
    };

    for (int i = 0; i < 8; i++) {
        int newRow = row + knightMoves[i][0];
        int newCol = col + knightMoves[i][1];
        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
            ans |= (1ULL << (newRow * 8 + newCol));
        }
    }

    return ans;
}



int main(){
    printf("const unsigned long long int RAttacks[64] = {\n");

    for(int square = 0; square < 64; square++){
        printf("  0x%llx,\n", rBitBoard(square));
    }
    printf("};\n\n");

    

    printf("const unsigned long long int BAttacks[64] = {\n");

    for(int square = 0; square < 64; square++){
        printf("  0x%llx,\n", bBitBoard(square));
    }
    printf("};\n\n");

    printf("const unsigned long long int QAttacks[64] = {\n");

    for(int square = 0; square < 64; square++){
        uint64 ans = rBitBoard(square) | bBitBoard(square);
        printf("  0x%llx,\n", ans);
    }
    printf("};\n\n");

    printf("const unsigned long long int KAttacks[64] = {\n");

    for(int square = 0; square < 64; square++){
        printf("  0x%llx,\n", kBitBoard(square));
    }
    printf("};\n\n");


    printf("const unsigned long long int WPawnAttacks[64] = {\n");

    for(int square = 0; square < 64; square++){
        printf("  0x%llx,\n", wPawnBitBoard(square));
    }
    printf("};\n\n");

    printf("const unsigned long long int BPawnAttacks[64] = {\n");

    for(int square = 0; square < 64; square++){
        printf("  0x%llx,\n", bPawnBitBoard(square));
    }
    printf("};\n\n");

    printf("const unsigned long long int NAttacks[64] = {\n");

    for(int square = 0; square < 64; square++){
        printf("  0x%llx,\n", nBitBoard(square));
    }
    printf("};\n\n");

    
}