from pieces.piece import Piece

class Pawn(Piece):
    def __init__(self, color, position, image, controller):
        super().__init__(color, position, image, controller)

    def move_options(self, white_pieces, black_pieces, turn):
        optional_moves = []
        if self.whose_piece == "white":
            ## if pawn is at starting position, it can move 2 spaces
            if self.position[1] == 6:
                optional_moves.append((self.position[0], self.position[1] - 2))
            optional_moves.append((self.position[0], self.position[1] - 1))
        else:
            if self.position[1] == 1:
                optional_moves.append((self.position[0], self.position[1] + 2))
            optional_moves.append((self.position[0], self.position[1] + 1))
        return optional_moves
