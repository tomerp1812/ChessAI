from board import Board
from controller import Controller
from game import Game


def main():
    width = 800
    height = 800
    num_of_squares = 64
    controller = Controller(width, height)
    board = Board(width, height, num_of_squares, controller)
    game = Game(controller, board)
    game.run_game()
    
if __name__ == "__main__":
    main()