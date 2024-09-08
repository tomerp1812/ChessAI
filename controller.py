import pygame

class Controller:
    _self = None

    def __new__(cls, WIDTH, HEIGHT):
        if cls._self is None:
            cls._self = super(Controller, cls).__new__(cls)
            cls._self.__init__(WIDTH, HEIGHT)
        return cls._self
    
    def __init__(self, WIDTH, HEIGHT):
        self.pygame = pygame
        self.pygame.init()
        self.screen = pygame.display.set_mode([WIDTH, HEIGHT])
        self.screen.fill('saddlebrown')
        self.clock = pygame.time.Clock()
        self.font = pygame.font.Font('freesansbold.ttf', 20)
        self.medium_font = pygame.font.Font('freesansbold.ttf', 40)
        self.big_font = pygame.font.Font('freesansbold.ttf', 50)
        self.init_images()
        
    def init_images(self):
        self.images = {}
        self.images["white_king"] = self.pygame.image.load("./pieces_images/white-king.png")
        self.images["white_queen"] = self.pygame.image.load("./pieces_images/white-queen.png")
        self.images["white_rook"] = self.pygame.image.load("./pieces_images/white-rook.png")
        self.images["white_bishop"] = self.pygame.image.load("./pieces_images/white-bishop.png")
        self.images["white_knight"] = self.pygame.image.load("./pieces_images/white-knight.png")
        self.images["white_pawn"] = self.pygame.image.load("./pieces_images/white-pawn.png")
        
        self.images["black_king"] = self.pygame.image.load("./pieces_images/black-king.png")
        self.images["black_queen"] = self.pygame.image.load("./pieces_images/black-queen.png")
        self.images["black_rook"] = self.pygame.image.load("./pieces_images/black-rook.png")
        self.images["black_bishop"] = self.pygame.image.load("./pieces_images/black-bishop.png")
        self.images["black_knight"] = self.pygame.image.load("./pieces_images/black-knight.png")
        self.images["black_pawn"] = self.pygame.image.load("./pieces_images/black-pawn.png")