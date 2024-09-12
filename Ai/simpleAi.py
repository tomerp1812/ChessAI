from Ai.ai import Ai
import numpy as np
import copy
from pieces.pawn import Pawn
from pieces.knight import Knight
from pieces.bishop import Bishop
from pieces.rook import Rook
from pieces.queen import Queen
from pieces.king import King

class SimpleAi(Ai):
    def __init__(self):
        super().__init__()
        
    def create_new_piece(self, piece):
        if piece.type_to_string() == "Pawn":
            new_piece = Pawn(piece.position, piece.whose_piece)
        elif piece.type_to_string() == "Knight":
            new_piece = Knight(piece.position, piece.whose_piece)
        elif piece.type_to_string() == "Bishop":
            new_piece = Bishop(piece.position, piece.whose_piece)
        elif piece.type_to_string() == "Queen":
            new_piece = Queen(piece.position, piece.whose_piece)
        elif piece.type_to_string() == "Rook":
            new_piece = Rook(piece.position, piece.whose_piece)
            new_piece.init_first_move(piece.rook_first_move)
        elif piece.type_to_string() == "King":
            new_piece = King(piece.position, piece.whose_piece)
            new_piece.init_first_move(piece.king_first_move)
        new_piece.optional_moves = piece.optional_moves
        return new_piece
    
    def create_new_pieces(self, pcs, pieces):
        rooks = []
        king = None
        index = 0
        i = 0
        for piece in pieces:
            new_piece = self.create_new_piece(piece)
            if new_piece.type_to_string() == "Rook":
                rooks.append(new_piece)
            elif new_piece.type_to_string() == "King":
                king = piece
                index = i
            pcs.append(self.create_new_piece(new_piece))
            i += 1
   
        if rooks[1].position == king.my_rooks_dictionary["short"].position:
            pcs[index].my_rooks(rooks[0], "short")
            pcs[index].my_rooks(rooks[1], "long")
        else:
            pcs[index].my_rooks(rooks[1], "short")
            pcs[index].my_rooks(rooks[0], "long")
        return pcs[index]
    
    def init_kings(self, piece):
        if piece.whose_piece == self.my_king.whose_piece:
            piece.init_my_king(self.my_king)
            piece.init_opponent_king(self.opp_king)
        else:
            piece.init_my_king(self.opp_king)
            piece.init_opponent_king(self.my_king)
        
    def create_position(self, my_pieces, opponent_pieces):
        my_king = self.create_new_pieces(self.my_pcs, my_pieces)
        opp_king = self.create_new_pieces(self.opp_pcs, opponent_pieces)
        
        self.my_king = my_king
        self.opp_king = opp_king
        
        for piece in self.my_pcs:
            self.init_kings(piece)
        for piece in self.opp_pcs:
            self.init_kings(piece)
    
    def restore(self, piece, other_piece, saved_optional_moves, index, other_index, my_pcs, opp_pcs):
        del my_pcs[index]
        my_pcs.insert(index, piece)
        if other_piece:
            if other_piece.whose_piece == piece.whose_piece:
                del my_pcs[other_index]
                my_pcs.insert(other_index, other_piece)
            else:
                opp_pcs.insert(other_index, other_piece)
        
        for key in saved_optional_moves:
            if saved_optional_moves[key][1] == "white":
                for piece in self.opp_pcs:
                    if piece.position == key:
                        piece.optional_moves = saved_optional_moves[key][0]
            else:
                for piece in self.my_pcs:
                    if piece.position == key:
                        piece.optional_moves = saved_optional_moves[key][0]
                
    def move(self, my_pieces, opponent_pieces, last_move):
        self.create_position(my_pieces, opponent_pieces)
        saved_optional_moves = self.update_move_options(last_move)
        value = -np.inf
        alpha = -np.inf
        beta = np.inf
        best_pcs = None
        opt_mov = None
        for piece in self.my_pcs:
            for move in piece.optional_moves:
                index = self.my_pcs.index(piece)
                other_piece, other_index = self.new_position(piece, move, last_move, self.my_pcs, self.opp_pcs)
                if other_piece and other_piece.whose_piece == piece.whose_piece:
                    last_move = self.my_pcs[-2], piece.position, move
                else:
                    last_move = self.my_pcs[-1], piece.position, move
                val = self.min_value(alpha, beta, 3, last_move)
                if val > value:
                    value = val
                    best_pcs = piece
                    opt_mov = move
                if value >= beta:
                    for my_piece in my_pieces:
                        if my_piece.position == best_pcs.position:
                            return my_piece, opt_mov
                alpha = max(alpha, value)
                self.restore(piece, other_piece, saved_optional_moves, index, other_index, self.my_pcs, self.opp_pcs)
                        
        print(value)
        for my_piece in my_pieces:
            if my_piece.position == best_pcs.position:
                return my_piece, opt_mov
    
    def update_move_options(self, last_move):
        saved_optional_moves = {}
        for piece in self.my_pcs:
            saved_optional_moves[piece.position] = piece.optional_moves, piece.whose_piece
            piece.move_options(self.opp_pcs, self.my_pcs, last_move)
        
        for opp in self.opp_pcs:
            saved_optional_moves[opp.position] = opp.optional_moves, opp.whose_piece
            opp.move_options(self.opp_pcs, self.my_pcs, last_move)
        return saved_optional_moves
        
    def new_position(self, piece, optional_move, last_move, my_pcs, opp_pcs):
        # move piece
        new_piece = self.create_new_piece(piece)
        self.init_kings(new_piece)
        new_piece.move(optional_move)
        index = my_pcs.index(piece)
        my_pcs.remove(piece)
        my_pcs.insert(index, new_piece)
        
        old_other_piece = None
        new_other_piece = None
        
        # Castling
        if piece.type_to_string() == "King":
            new_piece.init_first_move(False)
            if abs(piece.position[0] - optional_move[0]) == 2:
                if piece.position[0] > optional_move[0]:
                    old_other_piece = piece.my_rooks_dictionary["long"]
                    new_other_piece = self.create_new_piece(old_other_piece)
                    self.init_kings(new_other_piece)
                    new_other_piece.move((old_other_piece.position[0] + 3, old_other_piece.position[1]))
                else:
                    old_other_piece = piece.my_rooks_dictionary["short"]
                    new_other_piece = self.create_new_piece(old_other_piece)
                    self.init_kings(new_other_piece)
                    new_other_piece.move((old_other_piece.position[0] - 2, old_other_piece.position[1]))
                other_index = my_pcs.index(old_other_piece)
                my_pcs.remove(old_other_piece)
                my_pcs.insert(other_index, new_other_piece)
                return old_other_piece, other_index

        # En passant
        # if piece.type_to_string() == "Pawn":
        #     if last_move
        
        if piece.type_to_string() == "Rook":
            new_piece.init_first_move(False)
        
        for opp in opp_pcs:
            if opp.position == optional_move:
                other_index = opp_pcs.index(opp)
                opp_pcs.remove(opp)
                return opp, other_index
        
        return None, None
                
            
    # alpha starts at -inf, beta at inf
    def max_value(self, alpha, beta, depth, last_move):
        if self.terminal_state(self.opp_pcs, depth):
            return self.utility()
        value = -np.inf
        for piece in self.my_pcs:
            for optional_move in piece.optional_moves:
                index = self.my_pcs.index(piece)
                other_piece, other_index = self.new_position(piece, optional_move, last_move, self.my_pcs, self.opp_pcs)
                if other_piece and other_piece.whose_piece == piece.whose_piece:
                    last_move = self.my_pcs[index], piece.position, optional_move
                else:
                    last_move = self.my_pcs[index], piece.position, optional_move
                saved_optional_moves = self.update_move_options(last_move)
                val = self.min_value(alpha, beta, depth - 1, last_move)
                value = max(value, val)
                if value >= beta:
                    return value
                alpha = max(alpha, value)
                self.restore(piece, other_piece, saved_optional_moves, index, other_index, self.my_pcs, self.opp_pcs)
        return value
    
    def min_value(self, alpha, beta, depth, last_move):
        if self.terminal_state(self.my_pcs, depth):
            return self.utility(), None, None
        value = np.inf
        for piece in self.opp_pcs:
            for optional_move in piece.optional_moves:
                index = self.opp_pcs.index(piece)
                other_piece, other_index = self.new_position(piece, optional_move, last_move, self.opp_pcs, self.my_pcs)
                if other_piece and other_piece.whose_piece == piece.whose_piece:
                    last_move = self.my_pcs[-2], piece.position, optional_move
                else:
                    last_move = self.my_pcs[-1], piece.position, optional_move
                saved_optional_moves = self.update_move_options(last_move)
                val = self.max_value(alpha, beta, depth - 1, last_move)
                value = min(value, val)
                if value <= alpha:
                    return value
                beta = min(beta, value)
                self.restore(piece, other_piece, saved_optional_moves, index, other_index, self.opp_pcs, self.my_pcs)
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
        for piece in self.my_pcs:
            if piece.optional_moves:
                lost = False
                break
        if lost:
            return -np.inf
        for piece in self.opp_pcs:
            if piece.optional_moves:
                won = False
                break
        if won:
            return np.inf
    
        opp_val = self.evaluate(self.opp_pcs)
        my_val = self.evaluate(self.my_pcs)
        return my_val - opp_val
                
    def evaluate(self, pieces):
        val = 0
        for piece in pieces:
            if piece.type_to_string() == "Pawn":
                val += 1
            elif piece.type_to_string() == "Knight":
                val += 3
            elif piece.type_to_string() == "Bishop":
                val += 3
            elif piece.type_to_string() == "Rook":
                val += 5
            elif piece.type_to_string() == "Queen":
                val += 9
        return val