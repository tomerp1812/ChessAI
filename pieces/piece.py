class Piece():
    def __init__(self, color, position, image, controller):
        self.controller = controller
        self.color = color
        if self.color == (255, 255, 255):
            self.whose_piece = "white"
        else:
            self.whose_piece = "black"
        self.position = position
        self.image = self.controller.pygame.image.load(image)
        self.image = self.controller.pygame.transform.scale(self.image, (100, 100))
        self.optional_moves = []
        self.own_pieces_positions = []
        self.capturable_pieces_positions = []

    def draw_piece(self):
        scaled_position = (self.position[0] * 100, self.position[1] * 100)
        self.controller.screen.blit(self.image, scaled_position)

    def move(self, new_position):
        self.position = new_position

    def whose_pieces(self, white_pieces, black_pieces, turn):
        self.capturable_pieces_positions = []
        self.own_pieces_positions = []
        if turn == 'white':
            for black_piece in black_pieces:
                self.capturable_pieces_positions.append(black_piece.position)
            for white_piece in white_pieces:
                self.own_pieces_positions.append(white_piece.position)
        else:
            for black_piece in black_pieces:
                self.own_pieces_positions.append(black_piece.position)
            for white_piece in white_pieces:
                self.capturable_pieces_positions.append(white_piece.position)
    
    def check_move(self, move, new_position):
        if move and 0 <= new_position[0] <= 7 and 0 <= new_position[1] <= 7:
                if new_position in self.own_pieces_positions:
                    return False
                else:    
                    self.optional_moves.append(new_position)
                    if new_position in self.capturable_pieces_positions:
                        return False
                    return True
        return False
