from board import Board
from controller import Controller
from game import Game
from Ai.ai import Ai
from connector import Connector

def main():
    width = 800
    height = 800
    num_of_squares = 64
    controller = Controller(width, height)
    board = Board(width, height, num_of_squares, controller)
    game = Game(controller, board)
    ai = Ai()
    connector = Connector(ai, game, controller)
    connector.run_game()
    
if __name__ == "__main__":
    main()