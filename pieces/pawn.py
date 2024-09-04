from pieces.piece import Piece

class Pawn(Piece):
    def __init__(self, color, position, image, controller):
        super().__init__(color, position, image, controller)

    def check_pawn_move(self, optional):
        if optional not in self.own_pieces_positions and optional not in self.capturable_pieces_positions:
                self.optional_moves.append(optional)

    def check_capturing(self, optional):
        if optional in self.capturable_pieces_positions:
            self.optional_moves.append(optional)

    def move_options(self, white_pieces, black_pieces):
        self.optional_moves = []
        self.whose_pieces(white_pieces, black_pieces)
        if self.whose_piece == "white":
            ## if pawn is at starting position, it can move 2 spaces
            if self.position[1] == 6:
                self.check_pawn_move((self.position[0], self.position[1] - 2))
            self.check_pawn_move((self.position[0], self.position[1] - 1))
            self.check_capturing((self.position[0] + 1, self.position[1] - 1))
            self.check_capturing((self.position[0] - 1, self.position[1] - 1))
        else:
            if self.position[1] == 1:
                self.check_pawn_move((self.position[0], self.position[1] + 2))
            self.check_pawn_move((self.position[0], self.position[1] + 1))
            self.check_capturing((self.position[0] + 1, self.position[1] + 1))
            self.check_capturing((self.position[0] - 1, self.position[1] + 1))
        return self.optional_moves