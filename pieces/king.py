from pieces.piece import Piece
from check import am_i_in_check


class King(Piece):
    def __init__(self, color, position, image, controller):
        self.king_first_move = True
        self.my_rooks_dictionary = {}
        super().__init__(color, position, image, controller)
        
    def type_to_string(self):
        return "King"

    def my_rooks(self, rook, short_or_long):
        self.my_rooks_dictionary[short_or_long] = rook

    def move(self, new_position):
        self.king_first_move = False
        if abs(self.position[0] - new_position[0]) == 2:
            if self.position[0] > new_position[0]:
                rook = self.my_rooks_dictionary["long"]
                rook.move((rook.position[0] + 3, rook.position[1]))
            else:
                rook = self.my_rooks_dictionary["short"]
                rook.move((rook.position[0] - 2, rook.position[1]))
        super().move(new_position)

    def casteling_options(self, king_positions, occupied_positions, opponent_pieces, castle):
        for king_position in king_positions:
            if am_i_in_check(king_position, self.own_pieces_positions, opponent_pieces, self.whose_piece):
                return
            
        for white_piece in self.own_pieces_positions:
            if white_piece in occupied_positions:
                return
        
        self.optional_moves.append(castle)

    def move_options(self, white_pieces, black_pieces, last_move):
        self.optional_moves = []
        self.whose_pieces(white_pieces, black_pieces)
        self.check_move(
            True, (self.position[0] + 1, self.position[1]), white_pieces, black_pieces
        )
        self.check_move(
            True,
            (self.position[0] + 1, self.position[1] + 1),
            white_pieces,
            black_pieces,
        )
        self.check_move(
            True, (self.position[0], self.position[1] + 1), white_pieces, black_pieces
        )
        self.check_move(
            True,
            (self.position[0] - 1, self.position[1] + 1),
            white_pieces,
            black_pieces,
        )
        self.check_move(
            True, (self.position[0] - 1, self.position[1]), white_pieces, black_pieces
        )
        self.check_move(
            True,
            (self.position[0] - 1, self.position[1] - 1),
            white_pieces,
            black_pieces,
        )
        self.check_move(
            True, (self.position[0], self.position[1] - 1), white_pieces, black_pieces
        )
        self.check_move(
            True,
            (self.position[0] + 1, self.position[1] - 1),
            white_pieces,
            black_pieces,
        )

        if self.king_first_move and self.my_rooks_dictionary["long"].rook_first_move:
            if self.whose_piece == "white":
                self.casteling_options([(2, 7), (3, 7), (4, 7)], [(1, 7), (2, 7), (3, 7)], black_pieces, (2, 7))
            else:
                self.casteling_options([(2, 0), (3, 0), (4, 0)], [(1, 0), (2, 0), (3, 0)], white_pieces, (2, 0))

        if self.king_first_move and self.my_rooks_dictionary["short"].rook_first_move:
            if self.whose_piece == "white":
                self.casteling_options([(6, 7), (5, 7), (4, 7)], [(5, 7), (6, 7)], black_pieces, (6, 7))
            else:
                self.casteling_options([(6, 0), (5, 0), (4, 0)], [(5, 0), (6, 0)], white_pieces, (6, 0))

        return self.optional_moves
