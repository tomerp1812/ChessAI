class Connector:
    def __init__(self, ai, game, controller):
        self.controller = controller
        self.ai = ai
        self.game = game
        # self.game.pieces_creator(controller)
        self.running = True
        self.turn = "white"

    def get_position(self):
        self.pos = self.controller.pygame.mouse.get_pos()
        index_x = self.pos[0] // 100
        index_y = self.pos[1] // 100
        self.pos = (index_x, index_y)
        
    def run_game(self):
        while self.running:
            if self.turn == "white":
                for event in self.controller.pygame.event.get():
                    self.running = self.game.update_board()
                    if not self.running:
                        break
                    if event.type == self.controller.pygame.QUIT:
                        self.running = False
                        break
                    if self.game.piece:
                        if event.type == self.controller.pygame.MOUSEBUTTONDOWN:
                            self.running = self.game.click_move()
                            if not self.running:
                                break
                    if event.type == self.controller.pygame.MOUSEBUTTONDOWN:
                        self.game.pick()
                self.turn = self.game.turn
            else:
                self.running = self.game.update_board()
                if not self.running:
                    break
                piece, new_position = self.ai.move(self.game.black_pieces)
                self.game.piece = piece
                self.game.pos = new_position
                self.running = self.game.move()
                if not self.running:
                    break
                self.turn = self.game.turn
                
        
    