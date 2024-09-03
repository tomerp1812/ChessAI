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