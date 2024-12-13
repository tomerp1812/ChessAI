from pieces.piece import Piece
from check import am_i_in_check


class King(Piece):
    def __init__(self, position, color, image):
        self.my_rooks_dictionary = {}
        super().__init__(position, color, image)
          
    def init_first_move(self, king_first_move):
        self.king_first_move = king_first_move
        
    def type_to_string(self):
        return "King"

    def my_rooks(self, rook, short_or_long):
        self.my_rooks_dictionary[short_or_long] = rook

    def move(self, new_position):
        self.king_first_move = False
        castling = False
        old_rook_position = None
        if abs(self.position[0] - new_position[0]) == 2:
            if self.position[0] > new_position[0]:
                rook = self.my_rooks_dictionary["long"]
                old_rook_position = rook.position
                rook.move((rook.position[0] + 3, rook.position[1]))
                castling = True
            else:
                rook = self.my_rooks_dictionary["short"]
                old_rook_position = rook.position
                rook.move((rook.position[0] - 2, rook.position[1]))
                castling = True
        super().move(new_position)
        if castling:
            return rook.position, old_rook_position
        return None

    def castling_options(self, king_positions, occupied_positions, opponent_pieces, castle):
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
        king_move_options = [(1, 1), (1, 0), (1, -1), (0, 1), (0, -1), (-1, 1), (-1, 0), (-1, -1)]
        for king_move in king_move_options:
            new_position = (self.position[0] + king_move[0], self.position[1] + king_move[1])
            self.check_move(True, new_position, white_pieces, black_pieces)

        if "long" in self.my_rooks_dictionary:
            if self.king_first_move and self.my_rooks_dictionary["long"].rook_first_move:
                if self.whose_piece == "white":
                    self.castling_options([(2, 7), (3, 7), (4, 7)], [(1, 7), (2, 7), (3, 7)], black_pieces, (2, 7))
                else:
                    self.castling_options([(2, 0), (3, 0), (4, 0)], [(1, 0), (2, 0), (3, 0)], white_pieces, (2, 0))

        if "short" in self.my_rooks_dictionary:
            if self.king_first_move and self.my_rooks_dictionary["short"].rook_first_move:
                if self.whose_piece == "white":
                    self.castling_options([(6, 7), (5, 7), (4, 7)], [(5, 7), (6, 7)], black_pieces, (6, 7))
                else:
                    self.castling_options([(6, 0), (5, 0), (4, 0)], [(5, 0), (6, 0)], white_pieces, (6, 0))

        return self.optional_moves
