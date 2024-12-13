

class MyGame:
    def __init__(self, player1, player2, pieces, painter):
        self.player1 = player1
        self.player2 = player2 
        self.players = [self.player1, self.player2]
        self.playerId = 0
        self.pieces = pieces
        self.player1_pieces = pieces[0]
        self.player2_pieces = pieces[1]
        self.painter = painter
        self.running = True
        self.player1_pieces_dictionary = self.setDict(pieces[0])
        self.player2_pieces_dictionary = self.setDict(pieces[1])
        self.players_dictionaries = [self.player1_pieces_dictionary, self.player2_pieces_dictionary]
        self.currentPlayer = player1
        self.currentPiecesDictionary = self.player1_pieces_dictionary
        self.opponentPiecesDictionary = self.player2_pieces_dictionary
        self.piece = None
        
        
    def setDict(self, pieces):
        pieces_dictionary = {}
        for piece in pieces:
            pieces_dictionary[piece.getPosition()] = piece    
        return pieces_dictionary
    
    def run(self):
        while self.running:
            if not self.piece:
                position, self.running = self.currentPlayer.move()
                if position in self.currentPiecesDictionary:
                    self.piece = self.currentPiecesDictionary[position]
            else:
                position, self.running = self.currentPlayer.move()
                if position:
                    self.piece.move(position)
                    self.playerId = 1 - self.playerId
                    self.currentPlayer = self.players[self.playerId]
                    self.currentPiecesDictionary = self.players_dictionaries[self.playerId]
                    self.piece = None
                
            self.notify()
                
    # def run_human(self):
    #     if self.piece:
    #         self.currentPlayer.move()
    #     else:
    #         self.currentPlayer.click()
    
    # def run_Ai(self):
    #     pass
                             
                             
    def show_optional_moves(self, optional_moves):
        self.painter.show_optional_moves(optional_moves)
        
    def notify(self):
        self.painter.update()