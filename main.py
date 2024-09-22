from board import Board
from controller import Controller
from game import Game
from Ai.randomAi import RandomAi
from connector import Connector
from Ai.simpleAi import SimpleAi
from menu import Menu

def main():
    width = 800
    height = 800
    num_of_squares = 64
    controller = Controller(width, height)
    menu = Menu(controller)
    ai, human = menu.show()
    if not ai and not human:
        return
    board = Board(width, height, num_of_squares, controller)
    game = Game(controller, board)
    if not ai:
        connector = Connector(game, controller)
    else:    
        connector = Connector(game, controller, ai)
    connector.run_game()
    
if __name__ == "__main__":
    main()