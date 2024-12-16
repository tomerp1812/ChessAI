from pieces.piece import Piece

class Bishop(Piece):
    def __init__(self, position, color, image):
        super().__init__(position, color, image)
    
    def type_to_string(self):
        return "Bishop"

    def move_options(self):
        optional_moves = []

        for i in range(1, 8):
            optional_moves.append((self.position[0] + i, self.position[1] + i))
            optional_moves.append((self.position[0] + i, self.position[1] - i))
            optional_moves.append((self.position[0] - i, self.position[1] + i))
            optional_moves.append((self.position[0] - i, self.position[1] - i))

        return optional_moves