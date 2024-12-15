
from pieces.knight import Knight

class MyGame:
    def __init__(self, player1, player2, pieces, painter):
        self.player1 = player1
        self.player2 = player2 
        self.players = [self.player1, self.player2]
        self.playerId = 0
        self.pieces = pieces
        self.player1_pieces = pieces[0]
        self.player2_pieces = pieces[1]
        self.my_king = self.find_king(pieces[0])
        self.opp_king = self.find_king(pieces[1])
        self.my_rooks = self.find_rooks(pieces[0])
        self.opp_rooks = self.find_rooks(pieces[1])
        self.my_short_rook = self.my_rooks[1]
        self.my_long_rook = self.my_rooks[0]
        self.opp_short_rook = self.opp_rooks[1]
        self.opp_long_rook = self.opp_rooks[0]
        self.kings = [self.my_king, self.opp_king]
        self.painter = painter
        self.running = True
        self.currentPiecesDictionary = self.setDict(pieces[0])
        self.opponentPiecesDictionary = self.setDict(pieces[1])
        self.players_dictionaries = [self.currentPiecesDictionary, self.opponentPiecesDictionary]
        self.currentPlayer = player1
        self.piece = None
        self.optional_moves = None
        self.last_move = None
       
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
    
    
    def run(self):
        while self.running:
            self.notify(self.optional_moves)
            if not self.piece:
                position, self.running = self.currentPlayer.move()
                self.selectPiece(position)
            else:
                position, self.running = self.currentPlayer.move()
                if position and position in self.optional_moves:
                    self.updateState(position)
                else:
                    self.selectPiece(position) 
    
    def capture(self, position, old_position):
        if self.piece.type_to_string() == "Pawn":
            if self.checkEnPassant(position, old_position):
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
    def updateState(self, position):
        old_position = self.piece.getPosition()
        captured_piece = self.capture(position, old_position)
        # check if piece is captured
        if captured_piece is not None:
            del captured_piece
        
        self.currentPiecesDictionary.pop(old_position)
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
        self.playerId = 1 - self.playerId
        self.currentPlayer = self.players[self.playerId]
        self.currentPiecesDictionary = self.players_dictionaries[self.playerId]
        self.opponentPiecesDictionary = self.players_dictionaries[1 - self.playerId]
        self.my_king = self.kings[self.playerId]
        self.opp_king = self.kings[1 - self.playerId]
        self.swap_rooks()
        self.piece = None
        self.optional_moves = None

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
                    self.optional_moves = self.optionalMoves(position)
                    if not bool(self.optional_moves):
                        self.piece = None
                        self.optional_moves = None            
                    
    def optionalMoves(self, position):
        # retrieve all theoretical optional moves for that piece
        theoretical_moves = self.piece.move_options()
        optional_moves = []
        blockades = []
        
        for theoretical_move in theoretical_moves:
            if self.outOfBorders(theoretical_move):
                continue 
            
            elif theoretical_move in self.currentPiecesDictionary:
                blockades.append(theoretical_move)
                continue
            
            if self.piece.type_to_string() == "Pawn":
                if self.checkEnPassant(theoretical_move, position):
                    optional_moves.append(theoretical_move)
                    continue
                
                elif theoretical_move[0] != self.piece.getPosition()[0]:                
                    if theoretical_move not in self.opponentPiecesDictionary:
                        continue
                    
                # pawn can't walk into opponent piece 
                elif theoretical_move[0] == self.piece.getPosition()[0] and \
                    theoretical_move in self.opponentPiecesDictionary:
                        blockades.append(theoretical_move)
                        continue
                
            elif self.piece.type_to_string() == "King":    
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
                
        if self.piece.type_to_string() != "Knight":
            optional_moves = self.checkBlockades(optional_moves, blockades, position)
        
        legal_moves = []
        for optional_move in optional_moves:
            if self.checkCheck(optional_move, position):
                legal_moves.append(optional_move)
               
                
        return legal_moves   
        
      
    def checkEnPassant(self, move, position):
        if move[0] != self.piece.getPosition()[0]:
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
            if not self.checkCheck(option, position):
                return False
           
        return True
            
    def checkCheck(self, optional_move, position):
        piece = self.currentPiecesDictionary[position]
        self.currentPiecesDictionary.pop(position)
        self.currentPiecesDictionary[optional_move] = piece
        piece.setPosition(optional_move)
        capturedPiece = self.capture(optional_move, position)
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
                        valid = False
                    break
          
        
        if self.knightCheck():
            valid = False
              
        piece.setPosition(position)    
        self.currentPiecesDictionary.pop(optional_move)
        self.currentPiecesDictionary[position] = piece
        if capturedPiece:
            self.revertCapture(optional_move, capturedPiece)
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
    
    
    
    
    # def run_human(self):
    #     if self.piece:
    #         self.currentPlayer.move()
    #     else:
    #         self.currentPlayer.click()
    
    # def run_Ai(self):
    #     pass
                             
                             
    # def show_optional_moves(self, optional_moves):
    #     self.painter.show_optional_moves(optional_moves)
        
    def notify(self, optional_moves):
        self.painter.update(optional_moves)