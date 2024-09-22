# Chess AI

## Overview
This is a fully functional Chess game built with Pygame, offering both Human vs AI and Human vs Human gameplay. The game includes all the necessary rules and logic for chess, including advanced moves like **en passant**, **castling**, **threefold repetition**, and the **50-move rule**. Additionally, the game highlights possible moves when a player selects a piece, making it user-friendly for beginners.

## Features
- **Human vs AI** or **Human vs Human** gameplay
- Two AI options:
  - **Random AI**: Makes completely random moves.
  - **SimpleAI**: Uses the **minimax algorithm with alpha-beta pruning** to make strategic moves.
- Full support for all chess rules, including:
  - Normal chess moves: piece movement, captures, check, and checkmate.
  - Special rules: **en passant**, **castling**, **promotion**, **threefold repetition**, and **50-move rule**.
- **Move assistance for human players**: When a piece is selected, all possible moves are shown on the board.

<img src="https://github.com/user-attachments/assets/1c5862a0-2ab5-439b-a5a5-123465fd04e6" width="200" height="200">
<img src="https://github.com/user-attachments/assets/91a07078-5880-4809-abcf-cdb564f582a4" width="200" height="200">
<img src="https://github.com/user-attachments/assets/7ffa7b43-b5dd-4817-9e15-881ae0cafed8" width="200" height="200">
<img src="https://github.com/user-attachments/assets/fbe9cfae-5ec0-437f-ade8-46a40acf5506" width="200" height="200">


## Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/tomerp1812/ChessAI.git
   cd ChessAI
   ```
2. Install the dependencies:
   ```bash
   pipenv install
   ```
3. Run the game:
   ```bash
   pipenv run python main.py
   ```

## How to Play
1. Menu: After starting the game, you'll be presented with a menu where you can choose your opponent:
  * Random AI: Plays random moves.
  * Simple AI: Uses a more advanced strategy based on the minimax algorithm with alpha-beta pruning.
  * Human: Play against another human.
3. Game Controls:
  * Human players: Click on a piece to see its valid moves. Click again to move the piece to the desired square.
  * The game automatically handles check, checkmate, and draw conditions (threefold repetition, 50-move rule, stalemate).

## Contributors
https://github.com/tomerp1812
