from Ai.ai import Ai
import numpy as np
from pieces.pawn import Pawn
from pieces.knight import Knight
from pieces.bishop import Bishop
from pieces.rook import Rook
from pieces.queen import Queen
from pieces.king import King
import time


class SimpleAi(Ai):
    def __init__(self):
        super().__init__()
   
    def init(self, black_pieces, white_pieces):
        self.black_pieces = black_pieces
        self.white_pieces = white_pieces
        
        for piece in black_pieces:
            if piece.type_to_string() == "King":
                self.black_king = piece
                break
        
        for piece in white_pieces:
            if piece.type_to_string() == "King":
                self.white_king = piece
                break
            
    def restore(self, piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move, promotion = False, promoted = None, promoted_index = None):
        
        if promotion:
            if promoted.whose_piece == self.white_king.whose_piece:
                self.white_pieces.insert(promoted_index, piece)
                self.white_pieces.pop(-1)
            else:
                self.black_pieces.insert(promoted_index, piece)
                self.black_pieces.pop(-1)
            
        ### capture, castle are indexes
        if other_piece:
            if capture is not None:
                if other_piece.whose_piece == self.white_king.whose_piece:
                    self.white_pieces.insert(capture, other_piece)
                else:
                    self.black_pieces.insert(capture, other_piece)
            elif castle is not None:
                other_piece.rook_first_move = True
                if other_piece.whose_piece == self.white_king.whose_piece:
                    if other_piece.position == (5, 7):
                        other_piece.position = (7, 7)
                    elif other_piece.position == (3, 7):
                        other_piece.position = (0, 7)
                    else:
                        print("error in restore: castling")
                else:
                    if other_piece.position == (5, 0):
                        other_piece.position = (7, 0)
                    elif other_piece.position == (3, 0):
                        other_piece.position = (0, 0)
                    else:
                        print("error in restore: castling")
            else:
                print("Error in restore")
                
        piece.position = current_move[1]
        if piece.type_to_string() == "Rook":
            piece.rook_first_move = first_move
        elif piece.type_to_string() == "King":
            piece.king_first_move = first_move
                    
        # if piece.whose_piece == self.white_king.whose_piece:
        for white_piece in self.white_pieces:
            white_piece.optional_moves = saved_optional_moves[white_piece.position]
        # else:
        for black_piece in self.black_pieces:
            black_piece.optional_moves = saved_optional_moves[black_piece.position]

    def update_move_options(self, last_move):
        saved_optional_moves = {}

        # if last_move[0].whose_piece == "white":
        for black_piece in self.black_pieces:
            saved_optional_moves[black_piece.position] = black_piece.optional_moves
            black_piece.move_options(self.white_pieces, self.black_pieces, last_move)

        # elif last_move[0].whose_piece == "black":
        for white_piece in self.white_pieces:
            saved_optional_moves[white_piece.position] = white_piece.optional_moves
            white_piece.move_options(self.white_pieces, self.black_pieces, last_move)
                
        # else:
            # print("Error in update_move")

        return saved_optional_moves

    def check_castling(self, black_rook, white_rook, king):
        if king.whose_piece == self.black_king.whose_piece:
            for black_piece in self.black_pieces:
                if black_piece.position == black_rook:
                    return black_piece
        else:
            for white_piece in self.white_pieces:
                if white_piece.position == white_rook:
                    return white_piece
        return None

    def pieces_effected(self, piece, optional_move, last_move):
        captured_piece = None
        castling = None
        en_passant = None
        promotion = False
        
        if piece.whose_piece == self.black_king.whose_piece:
            # capture
            for white_piece in self.white_pieces:
                if white_piece.position == optional_move:
                    captured_piece = white_piece
                    break
        else:
            for black_piece in self.black_pieces:
                if black_piece.position == optional_move:
                    captured_piece = black_piece
                    break
        
        # castling
        if piece.type_to_string() == "King":
            if abs(piece.position[0] - optional_move[0]) == 2:
                # long castle
                if piece.position[0] > optional_move[0]:
                    castling = self.check_castling((0,0), (0,7), piece)
                    if castling is None or castling.type_to_string() != "Rook":
                        print("Error in pieces_effected: castling")
                        print(castling)
                # short castle
                else:
                    castling = self.check_castling((7,0), (7,7), piece)
                    if castling is None or castling.type_to_string() != "Rook":
                        print("Error in pieces_effected: castling")
                        print(castling)
        # en passant
        elif piece.type_to_string() == "Pawn":
            if last_move[0].type_to_string() == "Pawn":
                if last_move[0].whose_piece == "white":
                    if piece.position[1] == 4 and optional_move[1] == 5:
                        if (
                            last_move[1][1] == 6
                            and last_move[2][1] == 4
                            and abs(last_move[2][0] - piece.position[0]) == 1
                        ):
                            if optional_move[0] == last_move[2][0]:
                                for white_piece in self.white_pieces:
                                    if white_piece.position == last_move[2]:
                                        en_passant = white_piece
                                        break
                else:
                    if piece.position[1] == 3 and optional_move[1] == 2:
                        if (
                            last_move[1][1] == 1
                            and last_move[2][1] == 3
                            and abs(last_move[2][0] - piece.position[0]) == 1
                        ):
                            if optional_move[0] == last_move[2][0]:
                                for black_piece in self.black_pieces:
                                    if black_piece.position == last_move[2]:
                                        en_passant = black_piece
                                        break
        
        # promotion
        if piece.type_to_string() == "Pawn":
            if piece.whose_piece == self.black_king.whose_piece:
                if optional_move[1] == 7:
                    promotion = True
            else:
                if optional_move[1] == 0:
                    promotion = True
        
        return captured_piece, castling, en_passant, promotion 
 
    
    def update_position(self, piece, optional_move, captured_piece, castling, en_passant, promotion, promoted = None):
        first_move = None
        promoted_index = None
        if piece.type_to_string() == "Rook":
            first_move = piece.rook_first_move    
        elif piece.type_to_string() == "King":
            first_move = piece.king_first_move   
        piece.move(optional_move)
        
        if promotion:
            if piece.whose_piece == self.black_king.whose_piece:
                for i in range(len(self.black_pieces)):
                    if self.black_pieces[i].position == piece.position:
                        promoted_index = i
                        self.black_pieces.pop(i)
                        self.black_pieces.append(promoted)
                        break
            else:
                for i in range(len(self.white_pieces)):
                    if self.white_pieces[i].position == piece.position:
                        promoted_index = i
                        self.white_pieces.pop(i)
                        self.white_pieces.append(promoted)
                        break
        
        if captured_piece or en_passant:
            if captured_piece:
                to_remove = captured_piece
            else:
                to_remove = en_passant
            if to_remove.whose_piece == self.white_king.whose_piece:
                for i in range(len(self.white_pieces)):
                    if self.white_pieces[i].position == to_remove.position:
                        other_piece = self.white_pieces[i]
                        self.white_pieces.pop(i)
                        return other_piece, i, None, first_move, promoted_index
                print("Error in update_position: captured_piece")
            else:
                for i in range(len(self.black_pieces)):
                    if self.black_pieces[i].position == to_remove.position:
                        other_piece = self.black_pieces[i]
                        self.black_pieces.pop(i)
                        return other_piece, i, None, first_move, promoted_index
                print("Error in update_position: captured_piece")
        elif castling:
            if castling.whose_piece == self.white_king.whose_piece:
                for i in range(len(self.white_pieces)):
                    if self.white_pieces[i].position == castling.position:
                        return self.white_pieces[i], None, i, first_move, promoted_index
                print("Error in update_position: castling")
            else:
                for i in range(len(self.black_pieces)):
                    if self.black_pieces[i].position == castling.position:
                        return self.black_pieces[i], None, i, first_move, promoted_index
                print("Error in update_position: castling")
        return None, None, None, first_move, promoted_index
        
    def promotion_options(self, piece, optional_move):
        if piece.whose_piece == self.black_king.whose_piece:
            color = "black"
        else:
            color = "white"
            
        promotion_options = []
        promotion_options.append(Queen(
            optional_move,
            color,
        ))
        promotion_options.append(Rook(
            optional_move,
            color,
        ))
        promotion_options.append(Bishop(
            optional_move,
            color,
        ))
        promotion_options.append(Knight(
            optional_move,
            color,
        ))
        
        if color == "black":
            for piece in promotion_options:
                piece.init_my_king(self.black_king)
                piece.init_opponent_king(self.white_king)
        else:
            for piece in promotion_options:
                piece.init_my_king(self.white_king)
                piece.init_opponent_king(self.black_king)
                
        return promotion_options
    
    def move(self, black_pieces, white_pieces, last_move):
        t1 = time.time()
        self.init(black_pieces, white_pieces)
        saved_optional_moves = self.update_move_options(last_move)
        value = -np.inf
        alpha = -np.inf
        beta = np.inf
        best_pcs = None
        opt_mov = None
        promoted = None
        length = len(self.black_pieces)
        for i in range(length):
            piece = self.black_pieces[i]
            optional_moves = piece.optional_moves
            for optional_move in optional_moves:
                current_move = piece, piece.position, optional_move
                captured_piece, castling, en_passant, promotion  = self.pieces_effected(piece, optional_move, last_move)
                if promotion:
                    promotion_options = self.promotion_options(piece, optional_move)
                    for promotion_option in promotion_options:
                        other_piece, capture, castle, first_move, promoted_index = self.update_position(piece, optional_move, captured_piece, castling, en_passant, promotion, promotion_option)
                        val = self.min_value(alpha, beta, 1, current_move)
                        if val > value:
                            value = val
                            best_pcs = piece
                            opt_mov = optional_move
                            promoted = promotion_option
                        alpha = max(alpha, value)
                        self.restore(piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move, promotion, promotion_option, promoted_index)    
                else:    
                    other_piece, capture, castle, first_move, promoted_index = self.update_position(piece, optional_move, captured_piece, castling, en_passant, promotion)
                    val = self.min_value(alpha, beta, 1, current_move)
                    if val > value:
                        value = val
                        best_pcs = piece
                        opt_mov = optional_move
                        promoted = None
                    alpha = max(alpha, value)
                    self.restore(piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move)
        
        if best_pcs is None:
            for piece in self.black_pieces:
                if piece.optional_moves:
                    best_pcs = piece
                    opt_mov = piece.optional_moves[0]
                    break  
        t2 = time.time()
        print("Time taken: ", t2-t1)
        return best_pcs, opt_mov, promoted


    # alpha starts at -inf, beta at inf
    def max_value(self, alpha, beta, depth, last_move):
        value = -np.inf
        length = len(self.black_pieces)
        saved_optional_moves = self.update_move_options(last_move)
        if self.terminal_state(self.black_pieces, depth):
            return self.utility()
        for i in range(length):
            piece = self.black_pieces[i]
            optional_moves = piece.optional_moves
            for optional_move in optional_moves:
                current_move = piece, piece.position, optional_move
                captured_piece, castling, en_passant, promotion  = self.pieces_effected(piece, optional_move, last_move)
                if promotion:
                    promotion_options = self.promotion_options(piece, optional_move)
                    for promotion_option in promotion_options:
                        other_piece, capture, castle, first_move, promoted_index = self.update_position(piece, optional_move, captured_piece, castling, en_passant, promotion, promotion_option)
                        val = self.min_value(alpha, beta, depth - 1, current_move)
                        value = max(value, val)
                        if value >= beta:
                            self.restore(piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move, promotion, promotion_option, promoted_index)
                            return value
                        alpha = max(alpha, value)
                        self.restore(piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move, promotion, promotion_option, promoted_index)
                else:
                    other_piece, capture, castle, first_move, promoted_index = self.update_position(piece, optional_move, captured_piece, castling, en_passant, promotion)
                    val = self.min_value(alpha, beta, depth - 1, current_move)
                    value = max(value, val)
                    if value >= beta:
                        self.restore(piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move)
                        return value
                    alpha = max(alpha, value)
                    self.restore(piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move)
        return value

    def min_value(self, alpha, beta, depth, last_move):
        value = np.inf
        length = len(self.white_pieces)
        saved_optional_moves = self.update_move_options(last_move)
        if self.terminal_state(self.white_pieces, depth):
            return self.utility()
        for i in range(length):
            piece = self.white_pieces[i]
            optional_moves = piece.optional_moves
            for optional_move in optional_moves:
                current_move = piece, piece.position, optional_move
                captured_piece, castling, en_passant, promotion  = self.pieces_effected(piece, optional_move, last_move)
                if promotion:
                    promotion_options = self.promotion_options(piece, optional_move)
                    for promotion_option in promotion_options:
                        other_piece, capture, castle, first_move, promoted_index = self.update_position(piece, optional_move, captured_piece, castling, en_passant, promotion, promotion_option)
                        val = self.max_value(alpha, beta, depth - 1, current_move)
                        value = min(value, val)
                        if value <= alpha:
                            self.restore(piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move, promotion, promotion_option, promoted_index)
                            return value
                        beta = min(beta, value)
                        self.restore(piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move, promotion, promotion_option, promoted_index)
                else:
                    other_piece, capture, castle, first_move, promoted_index = self.update_position(piece, optional_move, captured_piece, castling, en_passant, promotion)
                    val = self.max_value(alpha, beta, depth - 1, current_move)
                    value = min(value, val)
                    if value <= alpha:
                        self.restore(piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move)
                        return value
                    beta = min(beta, value)
                    self.restore(piece, other_piece, capture, castle, first_move, saved_optional_moves, current_move)
        return value

    def terminal_state(self, pieces, depth):
        if depth == 0:
            return True

        for piece in pieces:
            if piece.optional_moves:
                return False
        return True

    def utility(self):
        lost = True
        won = True
        for piece in self.black_pieces:
            if piece.optional_moves:
                lost = False
                break

        if lost:
            for white_piece in self.white_pieces:
                if self.black_king.position in white_piece.optional_moves:
                    return -np.inf
            return 0

        for piece in self.white_pieces:
            if piece.optional_moves:
                won = False
                break
        if won:
            for black_piece in self.black_pieces:
                if self.white_king.position in black_piece.optional_moves:
                    return np.inf
            return 0

        opp_val = self.evaluate(self.white_pieces)
        my_val = self.evaluate(self.black_pieces)
        return my_val - opp_val

    def evaluate(self, pieces):
        val = 0
        for piece in pieces:
            val += piece.value
        return val
