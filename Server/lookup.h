extern const unsigned long long int RAttacks[64];
extern const unsigned long long int BAttacks[64];
extern const unsigned long long int QAttacks[64];
extern const unsigned long long int KAttacks[64];
extern const unsigned long long int WPAttacks[64];
extern const unsigned long long int BPAttacks[64];
extern const unsigned long long int NAttacks[64];


// evaluation function of piece according to their standings
extern const double (*white_mg_table[6])[64]; // mg is midle game
extern const double (*white_eg_table[6])[64]; // eg is end game
extern const double (*black_mg_table[6])[64];
extern const double (*black_eg_table[6])[64];

extern const double white_mg_passed_pawn_table[64];
extern const double white_eg_passed_pawn_table[64];
extern const double black_mg_passed_pawn_table[64];
extern const double black_eg_passed_pawn_table[64];

extern const unsigned long long int zobristTable[12][64]; // 12 pieces (6 each for white/black) x 64 squares
extern const unsigned long long int zobristCastling[4];  // Castling rights
extern const unsigned long long int zobristEnPassant[8]; // En-passant targets (one per file)
extern const unsigned long long int zobristTurn;         // Turn


extern const unsigned long long int whiteForwardMasks[8];
extern const unsigned long long int blackForwardMasks[8];

extern const double inf; // inf
extern const double negInf; // -inf