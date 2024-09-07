from pieces.piece import Piece
from check import am_i_in_check

class Pawn(Piece):
    def __init__(self, color, position, image, controller):
        super().__init__(color, position, image, controller)
    
    def type_to_string(self):
        return "Pawn"

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

    def an_passant(self, last_move, opponent_pieces, pos):

        if self.position[1] == pos[2]:
            if last_move[0].type_to_string() == "Pawn" and last_move[1][1] == pos[0] and last_move[2][1] == pos[2] and abs(last_move[2][0] - self.position[0]) == 1:
                old_position = self.position
                self.position = (last_move[2][0], pos[1])
                self.own_pieces_positions.remove(old_position)
                self.own_pieces_positions.append(self.position)
                pawn_removed = []
                for opponent_piece in opponent_pieces:
                    if opponent_piece.position != last_move[2]:
                        pawn_removed.append(opponent_piece)
                if not am_i_in_check(self.my_king.position, self.own_pieces_positions, pawn_removed, self.whose_piece):
                    self.optional_moves.append(self.position)
                self.position = old_position
                self.own_pieces_positions.remove((last_move[2][0], pos[1]))
                self.own_pieces_positions.append(old_position)

    def move_options(self, white_pieces, black_pieces, last_move):
        self.optional_moves = []
        self.whose_pieces(white_pieces, black_pieces)
        if self.whose_piece == "white":
            ## if pawn is at starting position, it can move 2 spaces
            if self.position[1] == 6:
                self.check_pawn_move((self.position[0], self.position[1] - 2), white_pieces, black_pieces)
            self.check_pawn_move((self.position[0], self.position[1] - 1), white_pieces, black_pieces)
            self.check_capturing((self.position[0] + 1, self.position[1] - 1), white_pieces, black_pieces)
            self.check_capturing((self.position[0] - 1, self.position[1] - 1), white_pieces, black_pieces)
            self.an_passant(last_move, black_pieces, (1, 2, 3))
        else:
            if self.position[1] == 1:
                self.check_pawn_move((self.position[0], self.position[1] + 2), white_pieces, black_pieces)
            self.check_pawn_move((self.position[0], self.position[1] + 1), white_pieces, black_pieces)
            self.check_capturing((self.position[0] + 1, self.position[1] + 1), white_pieces, black_pieces)
            self.check_capturing((self.position[0] - 1, self.position[1] + 1), white_pieces, black_pieces)
            self.an_passant(last_move, white_pieces, (6, 5, 4))
        return self.optional_moves