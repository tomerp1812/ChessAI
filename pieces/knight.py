from pieces.piece import Piece

class Knight(Piece):
    def __init__(self, color, position, image, controller):
        super().__init__(color, position, image, controller)

    def move_options(self, white_pieces, black_pieces, last_move):
        self.optional_moves = []
        self.whose_pieces(white_pieces, black_pieces)

        self.check_move(True, (self.position[0] + 1, self.position[1] + 2), white_pieces, black_pieces)
        self.check_move(True, (self.position[0] + 2, self.position[1] + 1), white_pieces, black_pieces)
        self.check_move(True, (self.position[0] + 2, self.position[1] - 1), white_pieces, black_pieces)
        self.check_move(True, (self.position[0] + 1, self.position[1] - 2), white_pieces, black_pieces)
        self.check_move(True, (self.position[0] - 1, self.position[1] - 2), white_pieces, black_pieces)
        self.check_move(True, (self.position[0] - 2, self.position[1] - 1), white_pieces, black_pieces)
        self.check_move(True, (self.position[0] - 2, self.position[1] + 1), white_pieces, black_pieces)
        self.check_move(True, (self.position[0] - 1, self.position[1] + 2), white_pieces, black_pieces)

        return self.optional_moves