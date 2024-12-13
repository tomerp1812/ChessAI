from check import am_i_in_check
# self, position, whose_piece, color = None, image = None, controller = None
class Piece:
    def __init__(self, position, color = None, image = None):
        self.color = color
        self.position = position
        self.image = image
        self.optional_moves = []
        self.own_pieces_positions = []
        self.capturable_pieces_positions = []

    def getPosition(self):
        return self.position
        
    def __eq__(self, other):
        return self.position == other.position
    
    def init_my_king(self, my_king):
        self.my_king = my_king

    def init_opponent_king(self, opponent_king):
        self.opponent_king = opponent_king

    # def draw_piece(self):
    #     scaled_position = (self.position[0] * 100, self.position[1] * 100)
    #     self.controller.screen.blit(self.image, scaled_position)

    def move(self, new_position):
        self.position = new_position
        ## not castling
        return None

    # def whose_pieces(self, white_pieces, black_pieces):
    #     self.capturable_pieces_positions = []
    #     self.own_pieces_positions = []
    #     if self.whose_piece == "white":
    #         for black_piece in black_pieces:
    #             self.capturable_pieces_positions.append(black_piece.position)
    #         for white_piece in white_pieces:
    #             self.own_pieces_positions.append(white_piece.position)
    #     else:
    #         for black_piece in black_pieces:
    #             self.own_pieces_positions.append(black_piece.position)
    #         for white_piece in white_pieces:
    #             self.capturable_pieces_positions.append(white_piece.position)

    # def check_move(self, move, new_position, white_pieces, black_pieces):
    #     if move and 0 <= new_position[0] <= 7 and 0 <= new_position[1] <= 7:
    #         if new_position in self.own_pieces_positions:
    #             return False
    #         else:
    #             old_position = self.position
    #             self.position = new_position
    #             self.own_pieces_positions.remove(old_position)
    #             self.own_pieces_positions.append(new_position)
    #             if self.whose_piece == "white":
    #                 opponent_pieces = black_pieces
    #             else:
    #                 opponent_pieces = white_pieces
    #             if not am_i_in_check(self.my_king.position, self.own_pieces_positions, opponent_pieces, self.whose_piece):
    #                 self.optional_moves.append(new_position)
    #             self.position = old_position
    #             self.own_pieces_positions.remove(new_position)
    #             self.own_pieces_positions.append(old_position)
    #             if new_position in self.capturable_pieces_positions:
    #                 return False
    #             return True
    #     return False

