from pieces.piece import Piece

class King(Piece):
    def __init__(self, color, position, image, controller):
        super().__init__(color, position, image, controller)
    
    def move_options(self, white_pieces, black_pieces, turn):
        self.optional_moves = []
        self.whose_pieces(white_pieces, black_pieces, turn)
        self.check_move(True, (self.position[0] + 1, self.position[1]))
        self.check_move(True, (self.position[0] + 1, self.position[1] + 1))
        self.check_move(True, (self.position[0], self.position[1] + 1))
        self.check_move(True, (self.position[0] - 1, self.position[1] + 1))
        self.check_move(True, (self.position[0] - 1, self.position[1]))
        self.check_move(True, (self.position[0] - 1, self.position[1] - 1))
        self.check_move(True, (self.position[0], self.position[1] - 1))
        self.check_move(True, (self.position[0] + 1, self.position[1] - 1))

        return self.optional_moves

