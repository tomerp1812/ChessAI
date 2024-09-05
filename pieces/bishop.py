from pieces.piece import Piece

class Bishop(Piece):
    def __init__(self, color, position, image, controller):
        super().__init__(color, position, image, controller)

    def move_options(self, white_pieces, black_pieces):
        self.optional_moves = []
        self.whose_pieces(white_pieces, black_pieces)
        up_right = True
        up_left = True
        down_right = True
        down_left = True

        for i in range(1, 8):
            up_right_position = self.position[0] + i, self.position[1] + i
            up_left_position = self.position[0] + i, self.position[1] - i
            down_right_position = self.position[0] - i, self.position[1] + i
            down_left_position = self.position[0] - i, self.position[1] - i

            up_right = self.check_move(up_right, up_right_position, white_pieces, black_pieces)
            up_left = self.check_move(up_left, up_left_position, white_pieces, black_pieces)
            down_right = self.check_move(down_right, down_right_position, white_pieces, black_pieces)
            down_left = self.check_move(down_left, down_left_position, white_pieces, black_pieces)
            
        return self.optional_moves