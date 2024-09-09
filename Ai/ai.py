import random

class Ai:
    def __init__(self):
        pass
    
    def move(self, pieces):         
        i = random.randint(0, len(pieces) - 1)
        while len(pieces[i].optional_moves) == 0:
            i = random.randint(0, len(pieces) - 1)
        j = random.randint(0, len(pieces[i].optional_moves) - 1)
        return pieces[i], pieces[i].optional_moves[j]