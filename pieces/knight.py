from pieces.piece import Piece

class Knight(Piece):
    def __init__(self, position, color, image):
        super().__init__(position, color, image)

    def type_to_string(self):
        return "Knight"
    
    def move_options(self):
        optional_moves = []
        knight_move_options = Knight.knight_move_options()
        
        for knight_move in knight_move_options:
            optional_moves.append((self.position[0] + knight_move[0], self.position[1] + knight_move[1]))
        
        # self.optional_moves = []
        # self.whose_pieces(white_pieces, black_pieces)
        # knight_move_options = [(1, 2), (2, 1), (2, -1), (1, -2), (-1, -2), (-2, -1), (-2, 1), (-1, 2)]
        # for knight_move in knight_move_options:
        #     new_position = (self.position[0] + knight_move[0], self.position[1] + knight_move[1])
        #     self.check_move(True, new_position, white_pieces, black_pieces)

        return optional_moves
    
    def knight_move_options():
        return [(1, 2), (2, 1), (2, -1), (1, -2), (-1, -2), (-2, -1), (-2, 1), (-1, 2)]