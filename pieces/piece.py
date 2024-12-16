from pieces.drawable import Drawable

class Piece(Drawable):
    def __init__(self, position, color = None, image = None):
        self.color = color
        self.position = position
        self.image = image
        self.alive = True

    def draw(self, screen):
        if self.alive:
            scaled_position = (self.position[0] * 100, self.position[1] * 100)
            screen.blit(self.image, scaled_position)
    
    def getColor(self):
        return self.color
    
    def setPosition(self, position):
        self.position = position
    
    def getPosition(self):
        return self.position
        
    def __eq__(self, other):
        return self.position == other.position

    def move(self, new_position):
        self.position = new_position