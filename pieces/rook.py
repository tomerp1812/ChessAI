from pieces.piece import Piece

class Rook(Piece):
    def __init__(self, position, color, image):
        self.rook_first_move = True
        super().__init__(position, color, image)
    
    def type_to_string(self):
        return "Rook"

    def move(self, new_position):
        self.rook_first_move = False
        super().move(new_position)

    def move_options(self):
        optional_moves = []

        for i in range(1, 8):
            optional_moves.append((self.position[0] + i, self.position[1]))
            optional_moves.append((self.position[0] - i, self.position[1]))
            optional_moves.append((self.position[0], self.position[1] + i))
            optional_moves.append((self.position[0], self.position[1] - i))
                  
        return optional_moves