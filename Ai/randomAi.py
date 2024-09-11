from Ai.ai import Ai
import random

class RandomAi(Ai):
    def __init__(self):
        super().__init__()
    
    def move(self, my_pieces, opponent_pieces, last_move):         
        i = random.randint(0, len(my_pieces) - 1)
        while len(my_pieces[i].optional_moves) == 0:
            i = random.randint(0, len(my_pieces) - 1)
        j = random.randint(0, len(my_pieces[i].optional_moves) - 1)
        return my_pieces[i], my_pieces[i].optional_moves[j]