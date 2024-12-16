from pieces.piece import Piece

class King(Piece):
    def __init__(self, position, color, image):
        self.king_first_move = True
        super().__init__(position, color, image)
        
    def type_to_string(self):
        return "King"

    def move(self, new_position):
        self.king_first_move = False
        super().move(new_position)

    def move_options(self):
        optional_moves = []
        king_moves = [(1, 1), (1, 0), (1, -1), (0, 1), (0, -1), (-1, 1), (-1, 0), (-1, -1)]
        for move in king_moves:
            optional_moves.append((self.position[0] + move[0], self.position[1] + move[1]))
            
        if self.king_first_move == True:
            optional_moves.append((self.position[0] + 2, self.position[1]))
            optional_moves.append((self.position[0] -2, self.position[1]))
            
        return optional_moves
