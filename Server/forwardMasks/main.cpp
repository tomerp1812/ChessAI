#include <iostream>
#include <bitset>

int main() {
    unsigned long long int whiteForwardMasks[8];
    unsigned long long int blackForwardMasks[8];

    // Initialize forward masks for white and black
    for (int rank = 0; rank < 8; ++rank) {
        whiteForwardMasks[rank] = ~((1ULL << (rank * 8)) - 1);
        blackForwardMasks[rank] = (1ULL << ((rank + 1) * 8)) - 1;
    }

    // Print the masks
    std::cout << "White Forward Masks:\n";
    for (int rank = 0; rank < 8; ++rank) {
        std::cout << "Rank " << rank << ": " << std::bitset<64>(whiteForwardMasks[rank]) << "\n";
    }

    std::cout << "\nBlack Forward Masks:\n";
    for (int rank = 0; rank < 8; ++rank) {
        std::cout << "Rank " << rank << ": " << std::bitset<64>(blackForwardMasks[rank]) << "\n";
    }

    return 0;
}
