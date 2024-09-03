class Piece:
    def __init__(self, color, position, image, controller):
        self.controller = controller
        self.color = color
        self.position = position
        self.image = self.controller.pygame.image.load(image)
        self.image = self.controller.pygame.transform.scale(self.image, (100, 100))
        self.optional_moves = []
        self.exists = True

    def draw_piece(self):
        if self.exists:
            self.controller.screen.blit(self.image, self.position)
            # self.controller.pygame.display.flip()