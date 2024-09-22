from Ai.ai import Ai
import random
import pieces.queen as Queen
class RandomAi(Ai):
    def __init__(self):
        super().__init__()
    
    def move(self, black_pieces, white_pieces, last_move):         
        i = random.randint(0, len(black_pieces) - 1)
        while len(black_pieces[i].optional_moves) == 0:
            i = random.randint(0, len(black_pieces) - 1)
        j = random.randint(0, len(black_pieces[i].optional_moves) - 1)
        if black_pieces[i].type_to_string() == "Pawn" and black_pieces[i].optional_moves[j][0] == 0:
            return black_pieces[i], black_pieces[i].optional_moves[j], Queen(black_pieces[i].optional_moves[j], "black")
        return black_pieces[i], black_pieces[i].optional_moves[j], None