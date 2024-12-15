from listener import Listener

class Painter(Listener):
    
    def __init__(self, pieces, controller, num_of_squares):
        self.pieces = pieces[0] + pieces[1]
        self.controller = controller
        self.num_of_squares = num_of_squares
    
    def getImage(self, piece_name):
        return self.controller.pygame.transform.scale(self.controller.images[piece_name], (100, 100))
        
    def addPiece(self, piece):
        self.pieces.append(piece)    
        
    def update(self, optional_moves = None):
        self.controller.screen.fill("saddlebrown")
        self.paintBoard()
        self.paintPieces()
        if optional_moves:
            self.show_optional_moves(optional_moves)
        self.controller.pygame.display.flip()

    
    def show_optional_moves(self, optional_moves):
        for optional_move in optional_moves:
            self.controller.pygame.draw.circle(
                        self.controller.screen,
                        "red",
                        (
                            optional_move[0] * 100 + 50,
                            optional_move[1] * 100 + 50,
                        ),
                        10,
                    )
        
    
    def paintPieces(self):
        for piece in self.pieces:
            piece.draw(self.controller.screen)
            
     # draw main game board
    def paintBoard(self):
        num_of_itterations = self.num_of_squares // 2
        for i in range(num_of_itterations):
            column = i % 4
            row = i // 4
            if row % 2 == 0:
                self.controller.pygame.draw.rect(self.controller.screen, 'sandybrown', [
                                600 - (column * 200), row * 100, 100, 100])
            else:
                self.controller.pygame.draw.rect(self.controller.screen, 'sandybrown', [
                                700 - (column * 200), row * 100, 100, 100])
                
                
    def draw_promotion_options(self, color):
        self.controller.screen.fill("white")
        self.controller.pygame.draw.rect(self.controller.screen, 'saddlebrown', [0, 0, 395, 395])
        self.controller.pygame.draw.rect(self.controller.screen, 'saddlebrown', [405, 0, 395, 395])
        self.controller.pygame.draw.rect(self.controller.screen, 'saddlebrown', [0, 405, 395, 395])
        self.controller.pygame.draw.rect(self.controller.screen, 'saddlebrown', [405, 405, 395, 395])
        
        if color == "white":
            self.controller.screen.blit(self.controller.pygame.transform.scale(self.controller.images["white_queen"], (300, 300)), (50, 50))
            self.controller.screen.blit(self.controller.pygame.transform.scale(self.controller.images["white_rook"], (300, 300)), (455, 50))
            self.controller.screen.blit(self.controller.pygame.transform.scale(self.controller.images["white_bishop"], (300, 300)), (50, 455))
            self.controller.screen.blit(self.controller.pygame.transform.scale(self.controller.images["white_knight"], (300, 300)), (455, 455))
        else:
            self.controller.screen.blit(self.controller.pygame.transform.scale(self.controller.images["black_queen"], (300, 300)), (50, 50))
            self.controller.screen.blit(self.controller.pygame.transform.scale(self.controller.images["black_rook"], (300, 300)), (455, 50))
            self.controller.screen.blit(self.controller.pygame.transform.scale(self.controller.images["black_bishop"], (300, 300)), (50, 455))
            self.controller.screen.blit(self.controller.pygame.transform.scale(self.controller.images["black_knight"], (300, 300)), (455, 455))
        
        self.controller.pygame.display.flip()