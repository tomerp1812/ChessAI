from pieces.piece import Piece
from check import am_i_in_check

class Pawn(Piece):
    def __init__(self, color, position, image, controller):
        super().__init__(color, position, image, controller)

    def check_pawn_move(self, optional, white_pieces, black_pieces):
        if optional not in self.own_pieces_positions and optional not in self.capturable_pieces_positions:
                old_position = self.position
                self.position = optional
                self.own_pieces_positions.remove(old_position)
                self.own_pieces_positions.append(optional)
                if self.whose_piece == "white":
                    opponent_pieces = black_pieces
                else:
                    opponent_pieces = white_pieces
                if not am_i_in_check(self.my_king.position, self.own_pieces_positions, opponent_pieces, self.whose_piece):
                    self.optional_moves.append(optional)
                self.position = old_position
                self.own_pieces_positions.remove(optional)
                self.own_pieces_positions.append(old_position)

    def check_capturing(self, optional, white_pieces, black_pieces):
        if optional in self.capturable_pieces_positions:
            old_position = self.position
            self.position = optional
            self.own_pieces_positions.remove(old_position)
            self.own_pieces_positions.append(optional)
            if self.whose_piece == "white":
                opponent_pieces = black_pieces
            else:
                opponent_pieces = white_pieces
            if not am_i_in_check(self.my_king.position, self.own_pieces_positions, opponent_pieces, self.whose_piece):
                self.optional_moves.append(optional)
            self.position = old_position
            self.own_pieces_positions.remove(optional)
            self.own_pieces_positions.append(old_position)

    def move_options(self, white_pieces, black_pieces):
        self.optional_moves = []
        self.whose_pieces(white_pieces, black_pieces)
        if self.whose_piece == "white":
            ## if pawn is at starting position, it can move 2 spaces
            if self.position[1] == 6:
                self.check_pawn_move((self.position[0], self.position[1] - 2), white_pieces, black_pieces)
            self.check_pawn_move((self.position[0], self.position[1] - 1), white_pieces, black_pieces)
            self.check_capturing((self.position[0] + 1, self.position[1] - 1), white_pieces, black_pieces)
            self.check_capturing((self.position[0] - 1, self.position[1] - 1), white_pieces, black_pieces)
        else:
            if self.position[1] == 1:
                self.check_pawn_move((self.position[0], self.position[1] + 2), white_pieces, black_pieces)
            self.check_pawn_move((self.position[0], self.position[1] + 1), white_pieces, black_pieces)
            self.check_capturing((self.position[0] + 1, self.position[1] + 1), white_pieces, black_pieces)
            self.check_capturing((self.position[0] - 1, self.position[1] + 1), white_pieces, black_pieces)
        return self.optional_moves