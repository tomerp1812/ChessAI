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
        self.exists = True

    def draw_piece(self):
        if self.exists:
            scaled_position = (self.position[0] * 100, self.position[1] * 100)
            self.controller.screen.blit(self.image, scaled_position)

    def move(self):
        pass

    def move_options(self):
        print("Piece")