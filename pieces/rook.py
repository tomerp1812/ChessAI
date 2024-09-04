from pieces.piece import Piece

class Rook(Piece):
    def __init__(self, color, position, image, controller):
        super().__init__(color, position, image, controller)

    def move_options(self, white_pieces, black_pieces):
        self.optional_moves = []
        self.whose_pieces(white_pieces, black_pieces)
        up = True
        down = True
        left = True
        right = True

        for i in range(1, 8):
            up_position = self.position[0] + i, self.position[1]
            down_position = self.position[0] - i, self.position[1]
            right_position = self.position[0], self.position[1] + i
            left_position = self.position[0], self.position[1] - i

            up = self.check_move(up, up_position)
            down = self.check_move(down, down_position)
            right = self.check_move(right, right_position)
            left = self.check_move(left, left_position)
            
        return self.optional_moves