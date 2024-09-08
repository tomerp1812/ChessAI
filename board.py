class Board:
    def __init__(self, width, height, num_of_squares, controller):
        self.width = width
        self.height = height
        self.num_of_squares = num_of_squares
        self.controller = controller


    # draw main game board
    def draw_board(self):
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