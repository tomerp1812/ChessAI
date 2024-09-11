from pieces.piece import Piece

class Knight(Piece):
    def __init__(self, position, whose_piece, color = None, image = None, controller = None):
        super().__init__(position, whose_piece, color, image, controller)

    def type_to_string(self):
        return "Knight"
    
    def move_options(self, white_pieces, black_pieces, last_move):
        self.optional_moves = []
        self.whose_pieces(white_pieces, black_pieces)
        knight_move_options = [(1, 2), (2, 1), (2, -1), (1, -2), (-1, -2), (-2, -1), (-2, 1), (-1, 2)]
        for knight_move in knight_move_options:
            new_position = (self.position[0] + knight_move[0], self.position[1] + knight_move[1])
            self.check_move(True, new_position, white_pieces, black_pieces)

        return self.optional_moves