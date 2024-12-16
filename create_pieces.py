from pieces.pawn import Pawn
from pieces.queen import Queen
from pieces.bishop import Bishop
from pieces.king import King
from pieces.knight import Knight
from pieces.rook import Rook

def create_pieces(controller):
       # self.image = self.controller.pygame.transform.scale(self.image, (100, 100))
    pieces = []
    white_pieces = []
    black_pieces = []
    white = "white"
    black = "black"
    for i in range(8):
        # create white pawns
        position = (i, 6)
        pawn = Pawn(
            position,
            white,
            controller.pygame.transform.scale(controller.images["white_pawn"], (100, 100))
        )
        white_pieces.append(pawn)
    rook1 = Rook(
        (0, 7), white, controller.pygame.transform.scale(controller.images["white_rook"], (100, 100))
    )
    knight1 = Knight(
        (1, 7), white, controller.pygame.transform.scale(controller.images["white_knight"], (100, 100))
    )
    bishop1 = Bishop(
        (2, 7), white, controller.pygame.transform.scale(controller.images["white_bishop"], (100, 100))
    )
    queen = Queen(
        (3, 7), white, controller.pygame.transform.scale(controller.images["white_queen"], (100, 100))
    )
    king = King(
        (4, 7), white, controller.pygame.transform.scale(controller.images["white_king"], (100, 100))
    )
    bishop2 = Bishop(
        (5, 7), white, controller.pygame.transform.scale(controller.images["white_bishop"], (100, 100))
    )
    knight2 = Knight(
        (6, 7), white, controller.pygame.transform.scale(controller.images["white_knight"], (100, 100))
    )
    rook2 = Rook(
        (7, 7), white, controller.pygame.transform.scale(controller.images["white_rook"], (100, 100))
    )
    white_pieces.extend(
        [rook1, knight1, bishop1, queen, king, bishop2, knight2, rook2]
    )

    for i in range(8):
        # create black pawns
        position = (i, 1)
        pawn = Pawn(
            position,
            black,
            controller.pygame.transform.scale(controller.images["black_pawn"], (100, 100))
        )
        black_pieces.append(pawn)

    black_rook1 = Rook(
        (0, 0), black, controller.pygame.transform.scale(controller.images["black_rook"], (100, 100))
    )
    black_knight1 = Knight(
        (1, 0), black, controller.pygame.transform.scale(controller.images["black_knight"], (100, 100))
    )
    black_bishop1 = Bishop(
        (2, 0), black, controller.pygame.transform.scale(controller.images["black_bishop"], (100, 100))
    )
    black_queen = Queen(
        (3, 0), black, controller.pygame.transform.scale(controller.images["black_queen"], (100, 100))
    )
    black_king = King(
        (4, 0), black, controller.pygame.transform.scale(controller.images["black_king"], (100, 100))
    )
    black_bishop2 = Bishop(
        (5, 0), black, controller.pygame.transform.scale(controller.images["black_bishop"], (100, 100))
    )
    black_knight2 = Knight(
        (6, 0), black, controller.pygame.transform.scale(controller.images["black_knight"], (100, 100))
    )
    black_rook2 = Rook(
        (7, 0), black, controller.pygame.transform.scale(controller.images["black_rook"], (100, 100))
    )
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

    pieces.append(white_pieces)
    pieces.append(black_pieces)
    return pieces