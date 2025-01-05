
from pieces.knight import Knight
from pieces.queen import Queen
from pieces.bishop import Bishop
from pieces.rook import Rook
import time

class Game:
    def __init__(self, player1, player2, pieces, painter, fen):
        self.fen = fen
        self.player1 = player1
        self.player2 = player2 
        self.players = [self.player1, self.player2]
        self.pieces = pieces
        self.player1_pieces = pieces[0]
        self.player2_pieces = pieces[1]
        if fen.turn == 'w':
            self.playerId = 0
            self.my_king = self.find_king(pieces[0])
            self.opp_king = self.find_king(pieces[1])
            self.my_rooks = self.find_rooks(pieces[0])
            self.opp_rooks = self.find_rooks(pieces[1])
            self.currentPiecesDictionary = self.setDict(pieces[0])
            self.opponentPiecesDictionary = self.setDict(pieces[1])
            self.currentPlayer = player1
            self.kings = [self.my_king, self.opp_king]
            self.players_dictionaries = [self.currentPiecesDictionary, self.opponentPiecesDictionary]
            if "K" not in self.fen.castling and "Q" not in self.fen.castling:
                self.my_king.king_first_move = False
            if "k" not in self.fen.castling and "q" not in self.fen.castling:
                self.opp_king.king_first_move = False
        elif fen.turn == 'b':
            self.playerId = 1
            self.my_king = self.find_king(pieces[1])
            self.opp_king = self.find_king(pieces[0])
            self.my_rooks = self.find_rooks(pieces[1])
            self.opp_rooks = self.find_rooks(pieces[0])
            self.currentPiecesDictionary = self.setDict(pieces[1])
            self.opponentPiecesDictionary = self.setDict(pieces[0])
            self.currentPlayer = player2
            self.kings = [self.opp_king, self.my_king]
            self.players_dictionaries = [self.opponentPiecesDictionary, self.currentPiecesDictionary]
            if "K" not in self.fen.castling and "Q" not in self.fen.castling:
                self.opp_king.king_first_move = False
            if "k" not in self.fen.castling and "q" not in self.fen.castling:
                self.my_king.king_first_move = False        
        else:
            print("error in playerId")
        
        self.my_long_rook = None
        self.my_short_rook = None
        self.opp_long_rook = None
        self.opp_short_rook = None
        if len(self.my_rooks) == 2:
            self.my_short_rook = self.my_rooks[1]
            self.my_long_rook = self.my_rooks[0]
        elif len(self.my_rooks) == 1:
            rookPos = self.my_rooks[0].getPosition()
            if fen.turn == 'w':
                # long castle white rook
                if rookPos == (0, 7):
                    self.my_long_rook = self.my_rooks[0]
                # short castle white rook
                elif rookPos == (7, 7):
                    self.my_short_rook = self.my_rooks[0]
            else:
                # long castle black rook
                if rookPos == (0, 0):
                    self.my_long_rook = self.my_rooks[0]
                # short castle black rook
                elif rookPos == (7, 0):
                    self.my_short_rook = self.my_rooks[0]
        if len(self.opp_rooks) == 2:
            self.opp_short_rook = self.opp_rooks[1]
            self.opp_long_rook = self.opp_rooks[0]
        elif len(self.opp_rooks) == 1:    
            rookPos = self.opp_rooks[0].getPosition()
            if fen.turn == 'w':
                # long castle white rook
                if rookPos == (0, 7):
                    self.opp_long_rook = self.opp_rooks[0]
                # short castle white rook
                elif rookPos == (7, 7):
                    self.opp_short_rook = self.opp_rooks[0]
            else:
                # long castle black rook
                if rookPos == (0, 0):
                    self.opp_long_rook = self.opp_rooks[0]
                # short castle black rook
                elif rookPos == (7, 0):
                    self.opp_short_rook = self.opp_rooks[0]
                    
        
        self.painter = painter
        self.running = True
        self.piece = None
        self.optional_moves = None
        self.last_move = None
        self.states = {}
        self.fiftyRuleMove = 0
        self.currentState = self.state()
        
       
    def find_king(self, pieces):
        for piece in pieces:
            if piece.type_to_string() == "King":
                return piece
        return None

    def find_rooks(self, pieces):
        rooks = []
        for piece in pieces:
            if piece.type_to_string() == "Rook":
                rooks.append(piece)
        return rooks
        
    def setDict(self, pieces):
        pieces_dictionary = {}
        for piece in pieces:
            pieces_dictionary[piece.getPosition()] = piece    
        return pieces_dictionary
    
    
    def state(self):
        current_pieces = sorted([(p.type_to_string(), p.getPosition(), p.color) for p in self.currentPiecesDictionary.values()])
        opponent_pieces = sorted([(p.type_to_string(), p.getPosition(), p.color) for p in self.opponentPiecesDictionary.values()])
        
        kingsMoves = [self.my_king.king_first_move, self.opp_king.king_first_move]
        
        rookMoves = [rook.rook_first_move for rook in self.my_rooks + self.opp_rooks]
        
        state = (tuple(current_pieces), tuple(opponent_pieces), tuple(kingsMoves), tuple(rookMoves))
        return state
    
    # main function
    def run(self):
        while self.running:
            self.notify(self.optional_moves)
            
            if self.currentPlayer.me() == "Human":
                self.runHuman()
            elif self.currentPlayer.me() == "Ai":
                self.runAi()
    
    def isEnd(self):
        if self.checkEnd():
            self.notify()
            time.sleep(3)
            if self.checkMate():
                print("Player Won!")
            else:
                print("Tie!")
            self.running = False
                    
    def runHuman(self):
        if not self.piece:
                position, self.running = self.currentPlayer.click()
                self.selectPiece(position)
        else:
            position, self.running = self.currentPlayer.click()
            if position and position in self.optional_moves:
                # num of repetitions
                self.updateState(position)
                self.isEnd()
            else:
                self.selectPiece(position) 
    
    def runAi(self):
        move = self.currentPlayer.move(self.fen.getFen())
        if move:
            # time.sleep(0.5)
            self.selectPiece(move[0])
            if(len(move) == 3):
                self.updateState(move[1], move[2])
            else:
                self.updateState(move[1])
            self.isEnd()
    
    # if player is in check and also has no legal moves he is in mate
    def checkMate(self):
        if self.checkCheck():
            return False
        
        # still optional moves so it is a tie
        for position, piece in list(self.currentPiecesDictionary.items()):
            if self.optionalMoves(position, piece):
                return False
        return True
       
    # if player has no legal moves it means the game is over                
    def checkEnd(self):
        # save last state
        numOfRepetitions = 0
        if self.currentState in self.states:
            numOfRepetitions = self.states[self.currentState]
        self.states[self.currentState] = numOfRepetitions + 1
        
        if numOfRepetitions == 3:
            return True
        
        if self.fiftyRuleMove == 50:
            return True
        
        for position, piece in list(self.currentPiecesDictionary.items()):
            if self.optionalMoves(position, piece):
                return False
        
        return True
    
    def capture(self, position, old_position, piece):
        if piece.type_to_string() == "Pawn":
            if self.checkEnPassant(position, old_position, piece):
                dist = 0
                if position[1] > old_position[1]:
                    dist = -1
                else:
                    dist = 1
                piece = self.opponentPiecesDictionary[(position[0], position[1] + dist)]
                piece.alive = False
                self.opponentPiecesDictionary.pop((position[0], position[1] + dist))
                return piece
                
        if position in self.opponentPiecesDictionary:
            piece = self.opponentPiecesDictionary[position]
            piece.alive = False
            self.opponentPiecesDictionary.pop(position)
            return piece
        return None
            
    def revertCapture(self, position, piece):
        self.opponentPiecesDictionary[position] = piece
        piece.alive = True  
            
    # happens only after a move is played!        
    def updateState(self, position, promotion = None):
        old_position = self.piece.getPosition()
        captured_piece = self.capture(position, old_position, self.piece)
        # check if piece is captured
        if captured_piece is not None:
            self.fiftyRuleMove = 0
            del captured_piece
        else:
            # fiftyRulemove
            if self.piece.type_to_string != "Pawn":
                self.fiftyRuleMove += 1
            else:
                self.fiftyRuleMove = 0
                
        self.currentPiecesDictionary.pop(old_position)
        promoted_piece = None
        if self.piece.type_to_string() == "Pawn" and (position[1] == 7 or position[1] == 0):
            promoted_piece = self.promotePawn(position, promotion)
            self.currentPiecesDictionary[position] = promoted_piece
            self.piece.alive = False
            del self.piece
            self.piece = promoted_piece
            self.painter.addPiece(promoted_piece)
        else:
            self.currentPiecesDictionary[position] = self.piece
            
        self.last_move = (self.piece, old_position)
      
        # check if castles
        if self.castle(position):
            # short castle
            if position[0] > self.piece.getPosition()[0]:
                self.currentPiecesDictionary.pop(self.my_short_rook.getPosition())
                self.currentPiecesDictionary[(position[0] - 1, position[1])] = self.my_short_rook
                self.my_short_rook.move((position[0] - 1, position[1]))
            # long castle
            else:
                self.currentPiecesDictionary.pop(self.my_long_rook.getPosition())
                self.currentPiecesDictionary[(position[0] + 1, position[1])] = self.my_long_rook
                self.my_long_rook.move((position[0] + 1, position[1]))
                
        self.piece.move(position)
        self.fen.update_move(self.last_move, promoted_piece)
        self.playerId = 1 - self.playerId
        self.currentPlayer = self.players[self.playerId]
        self.currentPiecesDictionary = self.players_dictionaries[self.playerId]
        self.opponentPiecesDictionary = self.players_dictionaries[1 - self.playerId]
        self.my_king = self.kings[self.playerId]
        self.opp_king = self.kings[1 - self.playerId]
        self.swap_rooks()
        self.currentState = self.state()
        self.piece = None
        self.optional_moves = None

    def promotePawn(self, position, promotion = None):
        if position[1] == 7:
            color = "black"
        else:
            color = "white"
        
        if promotion:
            if promotion == 'q':
                clicked = (0, 0)
            elif promotion == 'n':
                clicked = (4, 4)
            elif promotion == 'r':
                clicked = (4, 0)
            elif promotion == 'b':
                clicked = (0, 4)
        else:
            self.painter.draw_promotion_options(color)
            clicked, self.running = self.currentPlayer.click()
        
        if 0 <= clicked[0] <= 3 and 0 <= clicked[1] <= 3:
            #queen
            image = self.painter.getImage(color + "_queen")
            new_piece = Queen(position, color, image)
        elif 0 <= clicked[0] <= 3:
            #bishop
            image = self.painter.getImage(color + "_bishop")
            new_piece = Bishop(position, color, image)
        elif 0 <= clicked[1] <= 3:
            #rook
            image = self.painter.getImage(color + "_rook")
            new_piece = Rook(position, color, image)
        else:
            #knight
            image = self.painter.getImage(color + "_knight")
            new_piece = Knight(position, color, image)
        return new_piece
            

    def castle(self, position):
        if self.piece.type_to_string() == "King":
            if abs(position[0] - self.piece.getPosition()[0]) == 2:
                return True
        return False
    
    def swap_rooks(self):
        short = self.my_short_rook
        self.my_short_rook = self.opp_short_rook
        self.opp_short_rook = short
        
        long = self.my_long_rook
        self.my_long_rook = self.opp_long_rook
        self.opp_long_rook = long

    def selectPiece(self, position):   
        self.piece = None
        self.optional_moves = None
        
        if position in self.currentPiecesDictionary:
                    self.piece = self.currentPiecesDictionary[position]
                    self.optional_moves = self.optionalMoves(position, self.piece)
                    if not bool(self.optional_moves):
                        self.piece = None
                        self.optional_moves = None            
                    
    def optionalMoves(self, position, piece):
        # retrieve all theoretical optional moves for that piece
        theoretical_moves = piece.move_options()
        optional_moves = []
        blockades = []
        
        for theoretical_move in theoretical_moves:
            if self.outOfBorders(theoretical_move):
                continue 
            
            elif theoretical_move in self.currentPiecesDictionary:
                blockades.append(theoretical_move)
                continue
            
            if piece.type_to_string() == "Pawn":
                if self.checkEnPassant(theoretical_move, position, piece):
                    optional_moves.append(theoretical_move)
                    continue
                
                elif theoretical_move[0] != piece.getPosition()[0]:                
                    if theoretical_move not in self.opponentPiecesDictionary:
                        continue
                    
                # pawn can't walk into opponent piece 
                elif theoretical_move[0] == piece.getPosition()[0] and \
                    theoretical_move in self.opponentPiecesDictionary:
                        blockades.append(theoretical_move)
                        continue
                
            elif piece.type_to_string() == "King":    
                if theoretical_move[0] - 2 == position[0]:
                    # short castle
                    if not self.checkCastle(True, position):
                        continue
                    
                elif theoretical_move[0] + 2 == position[0]:
                    if not self.checkCastle(False, position):
                        continue
            else:  
                if theoretical_move in self.opponentPiecesDictionary:
                    blockades.append(theoretical_move)
                
            optional_moves.append(theoretical_move)   
                
        if piece.type_to_string() != "Knight":
            optional_moves = self.checkBlockades(optional_moves, blockades, position)
        
        legal_moves = []
        for optional_move in optional_moves:
            piece, capturedPiece = self.simulate_next_position(position, optional_move)
            if self.checkCheck():
                legal_moves.append(optional_move)
            self.revert_next_position(piece, position, optional_move, capturedPiece)
               
                
        return legal_moves   
        
    def simulate_next_position(self, position, optional_move):
        piece = self.currentPiecesDictionary[position]
        self.currentPiecesDictionary.pop(position)
        self.currentPiecesDictionary[optional_move] = piece
        piece.setPosition(optional_move)
        capturedPiece = self.capture(optional_move, position, piece)
        return piece, capturedPiece

    def revert_next_position(self, piece, position, optional_move, capturedPiece):
        piece.setPosition(position)    
        self.currentPiecesDictionary.pop(optional_move)
        self.currentPiecesDictionary[position] = piece
        if capturedPiece:
            self.revertCapture(optional_move, capturedPiece)
    
      
    def checkEnPassant(self, move, position, piece):
        if move[0] != piece.getPosition()[0]:
            # if pawn moved last move
            if self.last_move and self.last_move[0].type_to_string() == "Pawn":
                # if the pawn move exactly 2 squeres, i.e first move.
                if abs(self.last_move[0].getPosition()[1] - self.last_move[1][1]) == 2:
                    # if the pawns are adjesent.
                    if move[0] == self.last_move[1][0]:
                        # if my in the same y as the other pawn
                        if position[1] == self.last_move[0].getPosition()[1]:  
                            return True
        return False
    
    def checkCastle(self, isShort, position):
        # can't castle if king already moved
        if not self.my_king.king_first_move:
            return False
        if isShort:
            rook = self.my_short_rook
            path_to_castle = [position, (position[0] + 1, position[1]), (position[0] + 2, position[1])]
        else:
            rook = self.my_long_rook
            path_to_castle = [position, (position[0] - 1, position[1]), (position[0] - 2, position[1])]
            # in long castle the place right next to the rook just has to be empty
            if (position[0] - 3, position[1]) in self.currentPiecesDictionary or \
                (position[0] - 3, position[1]) in self.opponentPiecesDictionary:
                    return False
            
            
        # can't castle if no rook, and can't castle if rook already moved
        if rook is None or not rook.rook_first_move:
            return False
        
        for option in path_to_castle:
            # can't castle through my pieces
            if position != option:
                if option in self.currentPiecesDictionary or option in self.opponentPiecesDictionary:
                    return False
            # can't castle through check
            piece, capturedPiece = self.simulate_next_position(position, option)
            if not self.checkCheck():
                self.revert_next_position(piece, position, option, capturedPiece)
                return False
            self.revert_next_position(piece, position, option, capturedPiece)
           
        return True
            
    def checkCheck(self):
        valid = True
        directions = [
            (0, 1),   # up
            (0, -1),  # down
            (1, 0),   # right
            (-1, 0),  # left
            (1, 1),   # up-right
            (-1, 1),  # up-left
            (1, -1),  # down-right
            (-1, -1)  # down-left
        ]
        
        king_position = self.my_king.getPosition()
        
        for direction in directions: 
            if not valid:
                break   
            dx, dy = direction
            x, y = king_position
            
            while not self.outOfBorders((x,y)):
                x += dx
                y += dy
                
                if self.outOfBorders((x,y)):
                    break
                
                if (x, y) in self.currentPiecesDictionary:
                    break
                
                if (x, y) in self.opponentPiecesDictionary:
                    opp_piece = self.opponentPiecesDictionary[(x, y)]
                    if king_position in opp_piece.move_options():
                        if opp_piece.type_to_string() == "Pawn":
                            if abs(opp_piece.position[0] - king_position[0]) == 1 and \
                                abs(opp_piece.position[1] - king_position[1]) == 1:
                                    valid = False
                        else:
                            valid = False
                    break
          
        
        if self.knightCheck():
            valid = False
              
        return valid
         
         
    def knightCheck(self):
        knight_moves = Knight.knight_move_options()
        king_position = self.my_king.getPosition()
        for knight_move in knight_moves:
            new_position = (king_position[0] + knight_move[0], king_position[1] + knight_move[1])
            if new_position in self.opponentPiecesDictionary:
                if self.opponentPiecesDictionary[new_position].type_to_string() == "Knight":
                    return True
                
        return False
         
    def checkBlockades(self, optional_moves, blockades, position):
        legal_moves = []
        for optional_move in optional_moves:
            is_legal = True
            for blockade in blockades:
                # same row
                if blockade[0] == position[0] and blockade[0] == optional_move[0]:
                    if (optional_move[1] < blockade[1] < position[1]) or (optional_move[1] > blockade[1] > position[1]):
                        is_legal = False
                        break
                # same column
                elif blockade[1] == position[1] and blockade[1] == optional_move[1]:
                    if (optional_move[0] < blockade[0] < position[0]) or (optional_move[0] > blockade[0] > position[0]):
                        is_legal = False
                        break
                # same diagonal    
                elif abs(blockade[0] - position[0]) == abs(blockade[1] - position[1]) and \
                    abs(blockade[0] - optional_move[0]) == abs(blockade[1] - optional_move[1]):
                        if ((position[0] < blockade[0] < optional_move[0]) and (position[1] < blockade[1] < optional_move[1])) or \
                        ((position[0] > blockade[0] > optional_move[0]) and (position[1] > blockade[1] > optional_move[1])) or \
                        ((position[0] < blockade[0] < optional_move[0]) and (position[1] > blockade[1] > optional_move[1])) or \
                        ((position[0] > blockade[0] > optional_move[0]) and (position[1] < blockade[1] < optional_move[1])):
                            is_legal = False
                            break
                    
            if is_legal:
                legal_moves.append(optional_move)
        return legal_moves


    def outOfBorders(self, check):
        if check[0] < 0 or check[0] > 7 or check[1] < 0 or check[1] > 7:
            return True
        return False
        
    def notify(self, optional_moves = None):
        self.painter.update(optional_moves)