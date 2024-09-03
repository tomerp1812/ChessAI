from pieces.piece import Piece

class Knight(Piece):
    def __init__(self, color, position, image, controller):
        super().__init__(color, position, image, controller)

    def move_options(self):
        first_iter = []
        first_iter.append((self.position[0] + 2, self.position[1] + 1))
        first_iter.append((self.position[0] + 2, self.position[1] - 1))
        first_iter.append((self.position[0] - 2, self.position[1] + 1))
        first_iter.append((self.position[0] - 2, self.position[1] - 1))
        first_iter.append((self.position[0] + 1, self.position[1] + 2))
        first_iter.append((self.position[0] + 1, self.position[1] - 2))
        first_iter.append((self.position[0] - 1, self.position[1] + 2))
        first_iter.append((self.position[0] - 1, self.position[1] - 2))

        second_iter = []
        for option in first_iter:
            if 0 <= option[0] <= 7 and 0 <= option[1] <= 7:
                second_iter.append(option)

        return second_iter