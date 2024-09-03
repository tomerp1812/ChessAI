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