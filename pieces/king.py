from pieces.piece import Piece

class King(Piece):
    def __init__(self, color, position, image, controller):
        self.king_first_move = True
        self.my_rooks_dictionary = {}
        super().__init__(color, position, image, controller)
    
    def my_rooks(self, rook, short_or_long):
        self.my_rooks_dictionary[short_or_long] = rook

    def move(self, new_position):
        self.king_first_move = False
        if abs(self.position[0] - new_position[0]) == 2:
            if self.position[0] > new_position[0]:
                rook = self.my_rooks_dictionary["long"]
                rook.move((rook.position[0] + 3,rook.position[1]))
            else:
                rook = self.my_rooks_dictionary["short"]
                rook.move((rook.position[0] - 2,rook.position[1]))
        super().move(new_position)

    def move_options(self, white_pieces, black_pieces):
        self.optional_moves = []
        self.whose_pieces(white_pieces, black_pieces)
        self.check_move(True, (self.position[0] + 1, self.position[1]))
        self.check_move(True, (self.position[0] + 1, self.position[1] + 1))
        self.check_move(True, (self.position[0], self.position[1] + 1))
        self.check_move(True, (self.position[0] - 1, self.position[1] + 1))
        self.check_move(True, (self.position[0] - 1, self.position[1]))
        self.check_move(True, (self.position[0] - 1, self.position[1] - 1))
        self.check_move(True, (self.position[0], self.position[1] - 1))
        self.check_move(True, (self.position[0] + 1, self.position[1] - 1))

        if self.king_first_move and self.my_rooks_dictionary["long"].rook_first_move:
            long_castle = True
            if self.whose_piece == "white":
                for white_piece in self.own_pieces_positions:
                    if white_piece == (1,7) or white_piece == (2,7) or white_piece == (3,7):
                        long_castle = False
                if long_castle:
                    self.optional_moves.append((2,7))
            else:
                for black_piece in self.own_pieces_positions:
                    if black_piece == (1,0) or black_piece == (2,0) or black_piece == (3,0):
                        long_castle = False
                if long_castle:
                    self.optional_moves.append((2,0))

        if self.king_first_move and self.my_rooks_dictionary["short"].rook_first_move:
            short_castle = True
            if self.whose_piece == "white":
                for white_piece in self.own_pieces_positions:
                    if white_piece == (5,7) or white_piece == (6,7):
                        short_castle = False
                if short_castle:
                    self.optional_moves.append((6,7))
            else:        
                for black_piece in self.own_pieces_positions:
                    if black_piece == (5,0) or black_piece == (6,0):
                        short_castle = False
                if short_castle:
                    self.optional_moves.append((6,0))
        
        return self.optional_moves

