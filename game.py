from pieces.pawn import Pawn
from pieces.bishop import Bishop
from pieces.king import King
from pieces.queen import Queen
from pieces.knight import Knight
from pieces.rook import Rook


class Game:
    def __init__(self, controller, board):
        self.controller = controller
        self.board = board
        self.white_pieces, self.black_pieces = self.pieces_creator()
        self.running = True
        self.pos = None
        self.piece = None
        self.turn = "white"
        self.last_move = None

    def get_position(self):
        self.pos = self.controller.pygame.mouse.get_pos()
        index_x = self.pos[0] // 100
        index_y = self.pos[1] // 100
        self.pos = (index_x, index_y)

    def update_board(self):
        self.controller.screen.fill("saddlebrown")
        self.board.draw_board()
        for piece in self.white_pieces:
            piece.draw_piece()
            # if self.turn == 'white':
            piece.move_options(self.white_pieces, self.black_pieces, self.last_move)
        for piece in self.black_pieces:
            piece.draw_piece()
            # if self.turn == 'black':
            piece.move_options(self.white_pieces, self.black_pieces, self.last_move)

        if self.piece:
            for optional_positions in self.piece.optional_moves:
                self.controller.pygame.draw.circle(
                    self.controller.screen,
                    "red",
                    (
                        optional_positions[0] * 100 + 50,
                        optional_positions[1] * 100 + 50,
                    ),
                    10,
                )
        self.controller.pygame.display.flip()

    def piece_captured(self):
        if self.turn == "white":
            for black_piece in self.black_pieces:
                if self.pos == black_piece.position:
                    self.black_pieces.remove(black_piece)
                    return
            if (
                type(self.last_move[0]) == Pawn
                and self.last_move[1][1] == 3
                and self.last_move[2][1] == 2
                and abs(self.last_move[2][0] - self.last_move[1][0]) == 1
            ):
                for black_piece in self.black_pieces:
                    if black_piece.position == (self.last_move[2][0], 3):
                        self.black_pieces.remove(black_piece)
                        return
        else:
            for white_piece in self.white_pieces:
                if self.pos == white_piece.position:
                    self.white_pieces.remove(white_piece)
                    return
            if (
                type(self.last_move[0]) == Pawn
                and self.last_move[1][1] == 4
                and self.last_move[2][1] == 5
                and abs(self.last_move[2][0] - self.last_move[1][0]) == 1
            ):
                for white_piece in self.white_pieces:
                    if white_piece.position == (self.last_move[2][0], 4):
                        self.white_pieces.remove(white_piece)
                        return

    def promotion(self):
        if type(self.last_move[0]) == Pawn:
            if self.turn == "white":
                if self.last_move[2][1] == 0:
                    for white_piece in self.white_pieces:
                        if white_piece.position == self.last_move[1]:
                            self.white_pieces.remove(white_piece)
                            q = Queen(self.controller.pygame.Color(255, 255, 255), self.last_move[2], "./pieces_images/white-queen.png", self.controller)
                            self.white_pieces.append(q)
                            for piece in self.white_pieces:
                                if type(piece) == King:
                                    q.init_my_king(piece)
                            for piece in self.black_pieces:
                                if type(piece) == King:
                                    q.init_opponent_king(piece)
                            return True
            else:
                if self.last_move[2][1] == 7:
                    for black_piece in self.black_pieces:
                        if black_piece.position == self.last_move[1]:
                            self.black_pieces.remove(black_piece)
                            q = Queen(self.controller.pygame.Color(0, 0, 0), self.last_move[2], "./pieces_images/black-queen.png", self.controller)
                            self.black_pieces.append(q)
                            for piece in self.black_pieces:
                                if type(piece) == King:
                                    q.init_my_king(piece)
                            for piece in self.white_pieces:
                                if type(piece) == King:
                                    q.init_opponent_king(piece)
                            return True
        return False
    
    def run_game(self):
        while self.running:
            for event in self.controller.pygame.event.get():
                self.update_board()
                ## check if the user wants to quit the game
                if event.type == self.controller.pygame.QUIT:
                    self.running = False
                ## check if the user clicked on a piece
                if self.piece:
                    if event.type == self.controller.pygame.MOUSEBUTTONDOWN:
                        self.get_position()
                        if self.pos in self.piece.optional_moves:
                            self.last_move = self.piece, self.piece.position, self.pos
                            self.piece_captured()
                            if not self.promotion():
                                self.piece.move(self.pos)
                            self.turn = "black" if self.turn == "white" else "white"
                        self.piece = None
                else:
                    if event.type == self.controller.pygame.MOUSEBUTTONDOWN:
                        self.get_position()
                        self.piece = self.piece_selected()

    def piece_selected(self):
        index_x = self.pos[0]
        index_y = self.pos[1]
        if self.turn == "white":
            for white_piece in self.white_pieces:
                white_piece_x = white_piece.position[0]
                white_piece_y = white_piece.position[1]
                if index_x == white_piece_x and index_y == white_piece_y:
                    return white_piece
        else:
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
            pawn = Pawn(
                white_color, position, "./pieces_images/white-pawn.png", self.controller
            )
            white_pieces.append(pawn)
        rook1 = Rook(
            white_color, (0, 7), "./pieces_images/white-rook.png", self.controller
        )
        knight1 = Knight(
            white_color, (1, 7), "./pieces_images/white-knight.png", self.controller
        )
        bishop1 = Bishop(
            white_color, (2, 7), "./pieces_images/white-bishop.png", self.controller
        )
        queen = Queen(
            white_color, (3, 7), "./pieces_images/white-queen.png", self.controller
        )
        king = King(
            white_color, (4, 7), "./pieces_images/white-king.png", self.controller
        )
        bishop2 = Bishop(
            white_color, (5, 7), "./pieces_images/white-bishop.png", self.controller
        )
        knight2 = Knight(
            white_color, (6, 7), "./pieces_images/white-knight.png", self.controller
        )
        rook2 = Rook(
            white_color, (7, 7), "./pieces_images/white-rook.png", self.controller
        )
        king.my_rooks(rook1, "long")
        king.my_rooks(rook2, "short")
        white_pieces.extend(
            [rook1, knight1, bishop1, queen, king, bishop2, knight2, rook2]
        )

        for i in range(8):
            # create black pawns
            position = (i, 1)
            pawn = Pawn(
                black_color, position, "./pieces_images/black-pawn.png", self.controller
            )
            black_pieces.append(pawn)
        black_rook1 = Rook(
            black_color, (0, 0), "./pieces_images/black-rook.png", self.controller
        )
        black_knight1 = Knight(
            black_color, (1, 0), "./pieces_images/black-knight.png", self.controller
        )
        black_bishop1 = Bishop(
            black_color, (2, 0), "./pieces_images/black-bishop.png", self.controller
        )
        black_queen = Queen(
            black_color, (3, 0), "./pieces_images/black-queen.png", self.controller
        )
        black_king = King(
            black_color, (4, 0), "./pieces_images/black-king.png", self.controller
        )
        black_bishop2 = Bishop(
            black_color, (5, 0), "./pieces_images/black-bishop.png", self.controller
        )
        black_knight2 = Knight(
            black_color, (6, 0), "./pieces_images/black-knight.png", self.controller
        )
        black_rook2 = Rook(
            black_color, (7, 0), "./pieces_images/black-rook.png", self.controller
        )
        ## assosiate king to the rooks for casteling
        black_king.my_rooks(black_rook1, "long")
        black_king.my_rooks(black_rook2, "short")
        black_pieces.extend(
            [
                black_rook1,
                black_knight1,
                black_bishop1,
                black_queen,
                black_king,
                black_bishop2,
                black_knight2,
                black_rook2,
            ]
        )

        for white_piece in white_pieces:
            white_piece.init_my_king(king)
            white_piece.init_opponent_king(black_king)
        for black_piece in black_pieces:
            black_piece.init_my_king(black_king)
            black_piece.init_opponent_king(king)
        return white_pieces, black_pieces
