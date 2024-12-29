#include <iostream>
#include <random>
#include <iomanip>

// Declare Zobrist tables
unsigned long long zobristTable[12][64]; // 12 pieces x 64 squares
unsigned long long zobristCastling[4];  // Castling rights
unsigned long long zobristEnPassant[8]; // En-passant targets (one per file)
unsigned long long zobristTurn;         // Turn

void initializeZobrist() {
    // Use a consistent seed for reproducibility
    std::mt19937_64 rng(123456789); // A fixed seed value
    std::uniform_int_distribution<unsigned long long> dist(0, ~0ULL);

    // Initialize piece-square table
    for (int piece = 0; piece < 12; ++piece) {
        for (int square = 0; square < 64; ++square) {
            zobristTable[piece][square] = dist(rng);
        }
    }

    // Initialize castling rights
    for (int i = 0; i < 4; ++i) {
        zobristCastling[i] = dist(rng);
    }

    // Initialize en-passant targets
    for (int i = 0; i < 8; ++i) {
        zobristEnPassant[i] = dist(rng);
    }

    // Initialize turn
    zobristTurn = dist(rng);
}

void printZobristValues() {
    std::cout << "unsigned long long zobristTable[12][64] = {\n";
    for (int piece = 0; piece < 12; ++piece) {
        std::cout << "    {";
        for (int square = 0; square < 64; ++square) {
            std::cout << zobristTable[piece][square];
            if (square < 63) std::cout << ", ";
        }
        std::cout << "}";
        if (piece < 11) std::cout << ",\n";
    }
    std::cout << "\n};\n\n";

    std::cout << "unsigned long long zobristCastling[4] = {";
    for (int i = 0; i < 4; ++i) {
        std::cout << zobristCastling[i];
        if (i < 3) std::cout << ", ";
    }
    std::cout << "};\n\n";

    std::cout << "unsigned long long zobristEnPassant[8] = {";
    for (int i = 0; i < 8; ++i) {
        std::cout << zobristEnPassant[i];
        if (i < 7) std::cout << ", ";
    }
    std::cout << "};\n\n";

    std::cout << "unsigned long long zobristTurn = " << zobristTurn << ";\n";
}

int main() {
    initializeZobrist();
    printZobristValues();
    return 0;
}
