from pieces.pawn import Pawn
from pieces.rook import Rook
from pieces.knight import Knight
from pieces.bishop import Bishop
from pieces.queen import Queen
from pieces.king import King

class Game:
    def __init__(self, controller, board):
        self.controller = controller
        self.board = board
        self.white_pieces, self.black_pieces = self.pieces_creator()
        self.piece_picked = False
        self.running = True
        self.pos = None
        self.piece = None

    def get_position(self):
        self.pos = self.controller.pygame.mouse.get_pos()
        index_x = self.pos[0] // 100
        index_y = self.pos[1] // 100
        self.pos = (index_x, index_y)

    def run_game(self):
        while self.running:
            for event in self.controller.pygame.event.get():
                ## check if the user wants to quit the game
                if event.type == self.controller.pygame.QUIT:
                    self.running = False
                ## check if the user clicked on a piece
                if self.piece:
                    if event.type == self.controller.pygame.MOUSEBUTTONDOWN:
                        self.get_position()
                        if self.pos in self.piece.move_options():
                            self.piece.move(self.pos)
                        self.piece = None
                else:
                    if event.type == self.controller.pygame.MOUSEBUTTONDOWN:
                        self.get_position()
                        self.piece = self.piece_selected()
                
                self.controller.screen.fill('saddlebrown')
                self.board.draw_board()
                for piece in self.white_pieces:
                    piece.draw_piece()
                for piece in self.black_pieces:
                    piece.draw_piece()
                self.controller.pygame.display.flip()

    def piece_selected(self):
        index_x = self.pos[0]
        index_y = self.pos[1]
        for white_piece in self.white_pieces:
            white_piece_x = white_piece.position[0]
            white_piece_y = white_piece.position[1]
            if index_x == white_piece_x and index_y == white_piece_y:
                return white_piece
        for black_piece in self.black_pieces:
            black_piece_x = black_piece.position[0]
            black_piece_y = black_piece.position[1]
            if index_x == black_piece_x and index_y == black_piece_y:
                return black_piece
        return None    
    
    
    
    def pieces_creator(self):
        white_pieces = []
        black_pieces = []
        white_color = self.controller.pygame.Color(255, 255, 255)
        black_color = self.controller.pygame.Color(0, 0, 0)

        for i in range(8):
            # create white pawns
            position = (i, 6)
            pawn = Pawn(white_color, position, "./pieces_images/white-pawn.png", self.controller)
            white_pieces.append(pawn)
        rook1 = Rook(white_color, (0, 7), "./pieces_images/white-rook.png", self.controller)
        knight1 = Knight(white_color, (1, 7), "./pieces_images/white-knight.png", self.controller)
        bishop1 = Bishop(white_color, (2, 7), "./pieces_images/white-bishop.png", self.controller)
        queen = Queen(white_color, (3, 7), "./pieces_images/white-queen.png", self.controller)
        king = King(white_color, (4, 7), "./pieces_images/white-king.png", self.controller)
        bishop2 = Bishop(white_color, (5, 7), "./pieces_images/white-bishop.png", self.controller)
        knight2 = Knight(white_color, (6, 7), "./pieces_images/white-knight.png", self.controller)
        rook2 = Rook(white_color, (7, 7), "./pieces_images/white-rook.png", self.controller)
        white_pieces.extend([rook1, knight1, bishop1, queen, king, bishop2, knight2, rook2])

        for i in range(8):
            # create black pawns
            position = (i, 1)
            pawn = Pawn(black_color, position, "./pieces_images/black-pawn.png", self.controller)
            black_pieces.append(pawn)
        black_rook1 = Rook(black_color, (0, 0), "./pieces_images/black-rook.png", self.controller)
        black_knight1 = Knight(black_color, (1, 0), "./pieces_images/black-knight.png", self.controller)
        black_bishop1 = Bishop(black_color, (2, 0), "./pieces_images/black-bishop.png", self.controller)
        black_queen = Queen(black_color, (3, 0), "./pieces_images/black-queen.png", self.controller)
        black_king = King(black_color, (4, 0), "./pieces_images/black-king.png", self.controller)
        black_bishop2 = Bishop(black_color, (5, 0), "./pieces_images/black-bishop.png", self.controller)
        black_knight2 = Knight(black_color, (6, 0), "./pieces_images/black-knight.png", self.controller)
        black_rook2 = Rook(black_color, (7, 0), "./pieces_images/black-rook.png", self.controller)
        black_pieces.extend([black_rook1, black_knight1, black_bishop1, black_queen, black_king, black_bishop2, black_knight2, black_rook2])
        return white_pieces, black_pieces