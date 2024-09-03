from pieces.piece import Piece

class Bishop(Piece):
    def __init__(self, color, position, image, controller):
        super().__init__(color, position, image, controller)

    def move_options(self):
        first_iter = []
        for i in range(1, 8):
            first_iter.append((self.position[0] + i, self.position[1] + i))
            first_iter.append((self.position[0] - i, self.position[1] - i))
            first_iter.append((self.position[0] + i, self.position[1] - i))
            first_iter.append((self.position[0] - i, self.position[1] + i))

        second_iter = []
        for option in first_iter:
            if 0 <= option[0] <= 7 and 0 <= option[1] <= 7:
                second_iter.append(option)

        return second_iter