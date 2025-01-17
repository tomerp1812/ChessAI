# from board import Board
from controller import Controller
# from menu import Menu
from painter import Painter
from game import Game
from create_pieces import create_pieces
from players.human import Human
from players.ai import Ai
from fen import Fen

def main():
    width = 800
    height = 800
    num_of_squares = 64
    fen = Fen()
    controller = Controller(width, height)
    pieces = create_pieces(controller, fen)
    painter = Painter(pieces, controller, num_of_squares)
    # player1 = Ai()
    player1 = Human(controller)
    # player2 = Human(controller)
    player2 = Ai()
    game = Game(player1, player2, pieces, painter, fen)
    game.run()
    # menu = Menu(controller)
    # ai, human = menu.show()
    # if not ai and not human:
    #     return
    # board = Board(width, height, num_of_squares, controller)
    # game = Game(controller, board)
    # if not ai:
    #     connector = Connector(game, controller)
    # else:    
    #     connector = Connector(game, controller, ai)
    # connector.run_game()
    
if __name__ == "__main__":
    main()
    

