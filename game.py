from pieces.pawn import Pawn
from pieces.bishop import Bishop
from pieces.king import King
from pieces.queen import Queen
from pieces.knight import Knight
from pieces.rook import Rook
import time


class Game:
    def __init__(self, controller, board):
        self.controller = controller
        self.board = board
        self.white_pieces = []
        self.black_pieces = []
        self.state = []
        self.fifty_move_rule = 0
        self.game_history = {}
        self.pieces_creator()
        self.running = True
        self.pos = None
        self.piece = None
        self.turn = "white"
        self.last_move = None

    def init_white_king(self, king):
        self.white_king = king

    def init_black_king(self, king):
        self.black_king = king

    def get_position(self):
        self.pos = self.controller.pygame.mouse.get_pos()
        index_x = self.pos[0] // 100
        index_y = self.pos[1] // 100
        self.pos = (index_x, index_y)

    def game_over(self, color, own_pieces, opponent_pieces, my_king):
        game_over = True
        for own_piece in own_pieces:
            if own_piece.optional_moves:
                game_over = False
                break
        if game_over:
            for opponent_piece in opponent_pieces:
                if my_king.position in opponent_piece.optional_moves:
                    print(color, "wins on checkmate")
                    self.running = False
                    return True
            print("It's a draw")
            self.running = False
            return True
        return False

    def update_board(self):
        self.controller.screen.fill("saddlebrown")
        self.board.draw_board()
        for piece in self.white_pieces:
            piece.draw_piece()
            piece.move_options(self.white_pieces, self.black_pieces, self.last_move)

        for piece in self.black_pieces:
            piece.draw_piece()
            piece.move_options(self.white_pieces, self.black_pieces, self.last_move)

        if self.game_over(
            "White", self.black_pieces, self.white_pieces, self.black_king
        ) or self.game_over(
            "Black", self.white_pieces, self.black_pieces, self.white_king
        ):
            return

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
                    return True
            if (
                self.last_move[0].type_to_string() == "Pawn"
                and self.last_move[1][1] == 3
                and self.last_move[2][1] == 2
                and abs(self.last_move[2][0] - self.last_move[1][0]) == 1
            ):
                for black_piece in self.black_pieces:
                    if black_piece.position == (self.last_move[2][0], 3):
                        self.black_pieces.remove(black_piece)
                        return True
        else:
            for white_piece in self.white_pieces:
                if self.pos == white_piece.position:
                    self.white_pieces.remove(white_piece)
                    return True
            if (
                self.last_move[0].type_to_string() == "Pawn"
                and self.last_move[1][1] == 4
                and self.last_move[2][1] == 5
                and abs(self.last_move[2][0] - self.last_move[1][0]) == 1
            ):
                for white_piece in self.white_pieces:
                    if white_piece.position == (self.last_move[2][0], 4):
                        self.white_pieces.remove(white_piece)
                        return True
        return False

    def show_promotion_options(self, rgb, color):
        self.running_promotion = True
        self.controller.screen.fill("black")
        while self.running_promotion:
            for event in self.controller.pygame.event.get():
                self.board.draw_promotion_options(self.piece.whose_piece)
                self.controller.pygame.display.flip()
                if event.type == self.controller.pygame.MOUSEBUTTONDOWN:
                    clicked_pos = self.controller.pygame.mouse.get_pos()
                    if clicked_pos[0] < 390 and clicked_pos[1] < 390:
                        return Queen(
                            self.controller.pygame.Color(rgb),
                            self.last_move[2],
                            self.controller.images[color + "_queen"],
                            self.controller,
                        )
                    elif clicked_pos[0] > 400 and clicked_pos[1] < 390:
                        return Rook(
                            self.controller.pygame.Color(rgb),
                            self.last_move[2],
                            self.controller.images[color + "_rook"],
                            self.controller,
                        )
                    elif clicked_pos[0] < 390 and clicked_pos[1] > 400:
                        return Bishop(
                            self.controller.pygame.Color(rgb),
                            self.last_move[2],
                            self.controller.images[color + "_bishop"],
                            self.controller,
                        )
                    elif clicked_pos[0] > 400 and clicked_pos[1] > 400:
                        return Knight(
                            self.controller.pygame.Color(rgb),
                            self.last_move[2],
                            self.controller.images[color + "_knight"],
                            self.controller,
                        )

    def promotion(self):
        if self.last_move[0].type_to_string() == "Pawn":
            if self.turn == "white":
                if self.last_move[2][1] == 0:
                    for white_piece in self.white_pieces:
                        if white_piece.position == self.last_move[1]:
                            self.white_pieces.remove(white_piece)
                            new_piece = self.show_promotion_options((255, 255, 255), "white")
                            self.white_pieces.append(new_piece)
                            for piece in self.white_pieces:
                                if piece.type_to_string() == "King":
                                    new_piece.init_my_king(piece)
                            for piece in self.black_pieces:
                                if piece.type_to_string() == "King":
                                    new_piece.init_opponent_king(piece)
                            return True
            else:
                if self.last_move[2][1] == 7:
                    for black_piece in self.black_pieces:
                        if black_piece.position == self.last_move[1]:
                            self.black_pieces.remove(black_piece)
                            new_piece = self.show_promotion_options((0, 0, 0), "black")
                            self.black_pieces.append(new_piece)
                            for piece in self.black_pieces:
                                if piece.type_to_string() == "King":
                                    new_piece.init_my_king(piece)
                            for piece in self.white_pieces:
                                if piece.type_to_string() == "King":
                                    new_piece.init_opponent_king(piece)
                            return True
        return False

    def threefold_repetition(self):
        if tuple(self.state) in self.game_history:
            self.game_history[tuple(self.state)] += 1
            if self.game_history[tuple(self.state)] == 3:
                print("It's a draw, threefold repetition")
                self.running = False
                return True
        else:
            self.game_history[tuple(self.state)] = 1
        return False

    def update_state(self, promotion, capture):
        if capture:
            for i in range(len(self.state)):
                if self.state[i][0] == self.pos:
                    self.state.pop(i)
                    break
        
        self.state.pop(self.state.index((self.last_move[1], self.piece.whose_piece + " " + self.piece.type_to_string())))
        if promotion:
            for i in range(len(self.state)):
                if (self.state[i][0][0] == self.pos[0] and self.state[i][0][1] > self.pos[1]) or self.state[i][0][0] > self.last_move[2][0]:
                    self.state.insert(i, (self.pos, self.piece.whose_piece + " " + self.white_pieces[-1].type_to_string()))
                    break
        else:
            for i in range(len(self.state)):
                if (self.state[i][0][0] == self.pos[0] and self.state[i][0][1] > self.pos[1]) or self.state[i][0][0] > self.pos[0]:
                    self.state.insert(i, (self.pos, self.piece.whose_piece + " " + self.piece.type_to_string()))
                    break

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
                            capture = self.piece_captured()
                            promotion = self.promotion()
                            if not promotion:
                                self.piece.move(self.pos)
                            self.update_state(promotion, capture)
                            if self.threefold_repetition():
                                break
                            if self.piece.type_to_string() == "Pawn" or capture:
                                self.fifty_move_rule = 0
                            else:
                                self.fifty_move_rule += 1
                            ## counting for both players, 50 moves means each player moved 50 times, so 100 moves in total
                            if self.fifty_move_rule == 100:
                                print("It's a draw, fifty move rule")
                                self.running = False
                                break
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

    def compare(self, e):
        return e[0][0], e[0][1]

    def init_state(self):
        self.state = []
        for white_piece in self.white_pieces:
            self.state.append(
                (
                    white_piece.position,
                    white_piece.whose_piece + " " + white_piece.type_to_string(),
                )
            )
        for black_piece in self.black_pieces:
            self.state.append(
                (
                    black_piece.position,
                    black_piece.whose_piece + " " + black_piece.type_to_string(),
                )
            )
        self.state.sort(key=self.compare)
        
    def pieces_creator(self):
        white_color = self.controller.pygame.Color(255, 255, 255)
        black_color = self.controller.pygame.Color(0, 0, 0)

        for i in range(8):
            # create white pawns
            position = (i, 6)
            pawn = Pawn(
                white_color,
                position,
                self.controller.images["white_pawn"],
                self.controller,
            )
            self.white_pieces.append(pawn)
        rook1 = Rook(
            white_color, (0, 7), self.controller.images["white_rook"], self.controller
        )
        knight1 = Knight(
            white_color, (1, 7), self.controller.images["white_knight"], self.controller
        )
        bishop1 = Bishop(
            white_color, (2, 7), self.controller.images["white_bishop"], self.controller
        )
        queen = Queen(
            white_color, (3, 7), self.controller.images["white_queen"], self.controller
        )
        king = King(
            white_color, (4, 7), self.controller.images["white_king"], self.controller
        )
        bishop2 = Bishop(
            white_color, (5, 7), self.controller.images["white_bishop"], self.controller
        )
        knight2 = Knight(
            white_color, (6, 7), self.controller.images["white_knight"], self.controller
        )
        rook2 = Rook(
            white_color, (7, 7), self.controller.images["white_rook"], self.controller
        )
        king.my_rooks(rook1, "long")
        king.my_rooks(rook2, "short")
        self.white_pieces.extend(
            [rook1, knight1, bishop1, queen, king, bishop2, knight2, rook2]
        )

        for i in range(8):
            # create black pawns
            position = (i, 1)
            pawn = Pawn(
                black_color,
                position,
                self.controller.images["black_pawn"],
                self.controller,
            )
            self.black_pieces.append(pawn)

        black_rook1 = Rook(
            black_color, (0, 0), self.controller.images["black_rook"], self.controller
        )
        black_knight1 = Knight(
            black_color, (1, 0), self.controller.images["black_knight"], self.controller
        )
        black_bishop1 = Bishop(
            black_color, (2, 0), self.controller.images["black_bishop"], self.controller
        )
        black_queen = Queen(
            black_color, (3, 0), self.controller.images["black_queen"], self.controller
        )
        black_king = King(
            black_color, (4, 0), self.controller.images["black_king"], self.controller
        )
        black_bishop2 = Bishop(
            black_color, (5, 0), self.controller.images["black_bishop"], self.controller
        )
        black_knight2 = Knight(
            black_color, (6, 0), self.controller.images["black_knight"], self.controller
        )
        black_rook2 = Rook(
            black_color, (7, 0), self.controller.images["black_rook"], self.controller
        )
        ## assosiate king to the rooks for casteling
        black_king.my_rooks(black_rook1, "long")
        black_king.my_rooks(black_rook2, "short")
        self.black_pieces.extend(
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

        for white_piece in self.white_pieces:
            white_piece.init_my_king(king)
            white_piece.init_opponent_king(black_king)
        for black_piece in self.black_pieces:
            black_piece.init_my_king(black_king)
            black_piece.init_opponent_king(king)

        self.init_state()
        self.game_history[tuple(self.state)] = 1
        self.init_white_king(king)
        self.init_black_king(black_king)
