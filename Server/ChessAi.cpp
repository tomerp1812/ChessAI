#include "ChessAi.h"
#include "lookup.cpp"
#include "Logic.cpp"
#include<limits>
#include <iostream>

using namespace std;


ChessAi::ChessAi(PosRepresentations posRep){
    this->posRep = &posRep;
    //implementation
}

ChessAi::~ChessAi(){
    //implementation
}

double ChessAi::evaluate(){
    //implementation
    return 0.0;
}

bool ChessAi::terminate(vector<Move> optionalMoves, int depth){
    if(depth <= 0 || optionalMoves.size() == 0){
        return true;
    }
    return false;
}

MoveVal ChessAi::search(int* board){
    double alpha = -1 * numeric_limits<double>::infinity();
    double beta = numeric_limits<double>::infinity();
    double value = -1 * numeric_limits<double>::infinity();
    int depth = 3;
    Logic* logic = new Logic();
    vector<Move> optionalMoves = logic->getOptionalMoves();
    MoveVal moveVal;
    moveVal.move = "";
    for(Move move: optionalMoves){
        double val = minValue(alpha, beta, depth);

        if(moveVal.move == ""){
            moveVal.move = "somestring"; // change!!
            moveVal.value = val;

        }else if(moveVal.value < val){
            moveVal.move = "somestringElse"; // change!!!
            moveVal.value = val;
        }
        if(alpha < moveVal.value){
            alpha = moveVal.value;
        }
    }


    delete logic;
    return moveVal;
}

double ChessAi::maxValue(double alpha, double beta, unsigned int depth){
    double value = -1 * numeric_limits<double>::infinity();
    Logic* logic = new Logic();
    vector<Move> optionalMoves = logic->getOptionalMoves();

    if(terminate(optionalMoves, depth)){
        return evaluate();
    }

    for(Move move: optionalMoves){
        double val = minValue(alpha, beta, depth - 1);
        if(val >= value){
            value = val;
        }
        if(value >= beta){
            delete logic;
            return value;
        }
        if(value >= alpha){
            alpha = value;
        }
    }
    
    delete logic;
    return value;
    
}

double ChessAi::minValue(double alpha, double beta, unsigned int depth){
    double value = numeric_limits<double>::infinity();
    Logic* logic = new Logic();
    vector<Move> optionalMoves = logic->getOptionalMoves();

    if(terminate(optionalMoves, depth)){
        return evaluate();
    }

    for(Move move: optionalMoves){
        double val = maxValue(alpha, beta, depth - 1);
        if(val >= value){
            value = val;
        }
        if(value <= alpha){
            delete logic;
            return value;
        }
        if(beta <= value){
            beta = value;
        }
    }
    
    delete logic;
    return value;
}

string ChessAi::run(string state){
    int* board = this->posRep->fenToBoard(state);
    string bestMove = search(board).move;
    delete board;
    return bestMove;
}

//  # alpha starts at -inf, beta at inf
//     def max_value(self, alpha, beta, depth, last_move):
//         value = -np.inf
//         length = len(self.black_pieces)
//         saved_optional_moves = self.update_move_options(last_move)
//         if self.terminal_state(self.black_pieces, depth):
//             return self.utility()
//         for i in range(length):
//             piece = self.black_pieces[i]
//             optional_moves = piece.optional_moves
//             for optional_move in optional_moves:
//                 current_move = piece, piece.position, optional_move
//                 captured_piece, castling, en_passant, promotion  = self.pieces_effected(piece, optional_move, last_move)
//                 if promotion:
//                     promotion_options = self.promotion_options(piece, optional_move)
//                     for promotion_option in promotion_options:
//                         other_piece, capture, castle, first_move, promoted_index = self.update_position(piece, optional_move, captured_piece, castling, en_passant, promotion, promotion_option)
//                         val = self.min_value(alpha, beta, depth - 1, current_move)
//                         value = max(value, val)
//                         if value >= beta:
//                             self.restore(piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move, promotion, promotion_option, promoted_index)
//                             return value
//                         alpha = max(alpha, value)
//                         self.restore(piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move, promotion, promotion_option, promoted_index)
//                 else:
//                     other_piece, capture, castle, first_move, promoted_index = self.update_position(piece, optional_move, captured_piece, castling, en_passant, promotion)
//                     val = self.min_value(alpha, beta, depth - 1, current_move)
//                     value = max(value, val)
//                     if value >= beta:
//                         self.restore(piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move)
//                         return value
//                     alpha = max(alpha, value)
//                     self.restore(piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move)
//         return value

//     def min_value(self, alpha, beta, depth, last_move):
//         value = np.inf
//         length = len(self.white_pieces)
//         saved_optional_moves = self.update_move_options(last_move)
//         if self.terminal_state(self.white_pieces, depth):
//             return self.utility()
//         for i in range(length):
//             piece = self.white_pieces[i]
//             optional_moves = piece.optional_moves
//             for optional_move in optional_moves:
//                 current_move = piece, piece.position, optional_move
//                 captured_piece, castling, en_passant, promotion  = self.pieces_effected(piece, optional_move, last_move)
//                 if promotion:
//                     promotion_options = self.promotion_options(piece, optional_move)
//                     for promotion_option in promotion_options:
//                         other_piece, capture, castle, first_move, promoted_index = self.update_position(piece, optional_move, captured_piece, castling, en_passant, promotion, promotion_option)
//                         val = self.max_value(alpha, beta, depth - 1, current_move)
//                         value = min(value, val)
//                         if value <= alpha:
//                             self.restore(piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move, promotion, promotion_option, promoted_index)
//                             return value
//                         beta = min(beta, value)
//                         self.restore(piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move, promotion, promotion_option, promoted_index)
//                 else:
//                     other_piece, capture, castle, first_move, promoted_index = self.update_position(piece, optional_move, captured_piece, castling, en_passant, promotion)
//                     val = self.max_value(alpha, beta, depth - 1, current_move)
//                     value = min(value, val)
//                     if value <= alpha:
//                         self.restore(piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move)
//                         return value
//                     beta = min(beta, value)
//                     self.restore(piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move)
//         return value

// value = -np.inf
//         alpha = -np.inf
//         beta = np.inf




