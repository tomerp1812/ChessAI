from pieces.piece import Piece

class Pawn(Piece):
    def __init__(self, position, color, image):
        self.pawn_first_move = True
        super().__init__(position, color, image)
        
    def type_to_string(self):
        return "Pawn"
              
    def move(self, new_position):
        self.pawn_first_move = False
        super().move(new_position)

    def move_options(self):
        optional_moves = []
        
        if self.color == 'white':
            optional_moves.append((self.position[0], self.position[1] - 1))
            if self.pawn_first_move:
                optional_moves.append((self.position[0], self.position[1] - 2))
            optional_moves.append((self.position[0] + 1, self.position[1] - 1))
            optional_moves.append((self.position[0] - 1, self.position[1] - 1))
        else:
            optional_moves.append((self.position[0], self.position[1] + 1))
            # first move option
            if self.pawn_first_move:
                optional_moves.append((self.position[0], self.position[1] + 2))
            # capture
            optional_moves.append((self.position[0] + 1, self.position[1] + 1))
            optional_moves.append((self.position[0] - 1, self.position[1] + 1))
        
        return optional_moves