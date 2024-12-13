from pieces.piece import Piece

class Queen(Piece):
    def __init__(self, position, color, image):
        super().__init__(position, color, image)
        
    def type_to_string(self):
        return "Queen"

    def move_options(self):
        optional_moves = []
        # self.whose_pieces(white_pieces, black_pieces)
        # up = True
        # down = True
        # left = True
        # right = True
        # up_right = True
        # up_left = True
        # down_right = True
        # down_left = True

        for i in range(1, 8):
            optional_moves.append(self.position[0] + i, self.position[1])
            optional_moves.append(self.position[0] - i, self.position[1])
            optional_moves.append(self.position[0], self.position[1] + i)
            optional_moves.append(self.position[0], self.position[1] - i)
            optional_moves.append(self.position[0] + i, self.position[1] + i)
            optional_moves.append(self.position[0] + i, self.position[1] - i)
            optional_moves.append(self.position[0] - i, self.position[1] + i)
            optional_moves.append(self.position[0] - i, self.position[1] - i)
            # up_position = self.position[0] + i, self.position[1]
            # down_position = self.position[0] - i, self.position[1]
            # right_position = self.position[0], self.position[1] + i
            # left_position = self.position[0], self.position[1] - i
            # up_right_position = self.position[0] + i, self.position[1] + i
            # up_left_position = self.position[0] + i, self.position[1] - i
            # down_right_position = self.position[0] - i, self.position[1] + i
            # down_left_position = self.position[0] - i, self.position[1] - i

            # up = self.check_move(up, up_position, white_pieces, black_pieces)
            # down = self.check_move(down, down_position, white_pieces, black_pieces)
            # right = self.check_move(right, right_position, white_pieces, black_pieces)
            # left = self.check_move(left, left_position, white_pieces, black_pieces)
            # up_right = self.check_move(up_right, up_right_position, white_pieces, black_pieces)
            # up_left = self.check_move(up_left, up_left_position, white_pieces, black_pieces)
            # down_right = self.check_move(down_right, down_right_position, white_pieces, black_pieces)
            # down_left = self.check_move(down_left, down_left_position, white_pieces, black_pieces)
            
        return optional_moves