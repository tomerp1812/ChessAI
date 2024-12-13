from player import Player

class Ai(Player):
    def __init__(self):
        super().__init__()
        
    def me(self):
        return "Ai"
    
    def move(self, pieces):
        pass