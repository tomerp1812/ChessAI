

class Fen:
    def __init__(self, fen = None):
        if fen:
            self.fen = fen
        else:
            self.fen = self.createStartingPosition()
        self.board = self.fen_to_board()
        splitFen = self.fen.split()
        self.turn = splitFen[1]
        self.castling = splitFen[2]
        self.enPassant = splitFen[3]
        self.halfMove = int(splitFen[4])
        self.fullMove = int(splitFen[5])
    
    def getFen(self):
        return self.fen
    
    def createStartingPosition(self):
        return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    
    def fen_to_board(self):
        # get's the board pieces, each / means new row.
        rows = self.fen.split()[0].split("/")
        board = [list(self.expand_row(row)) for row in rows]
        return board
    
    def board_to_fen(self):
        return "/".join([self.compress_row(row) for row in self.board])
    
    def expand_row(self, row):
        # if it is number it means how many squares are empty and we put '.' there on all those squares
        # otherwise it is a piece.
        return "".join(char if not char.isdigit() else "." * int(char) for char in row)
    
    def compress_row(self, row):
        compressed = ""
        empty = 0
        for char in row:
            if char == ".":
                empty += 1
            else:
                if empty > 0:
                    compressed += str(empty)
                    empty = 0
                compressed += char
        if empty > 0:
            compressed += str(empty)
        return compressed
    
    
    # last_move =  (self.piece, old_position)
    def update_move(self, last_move, promotion=None):
        # Update board
        x1, y1 = last_move[1]
        x2, y2 = last_move[0].getPosition()
        piece = self.board[y1][x1]
        target = self.board[y2][x2]
        self.board[y1][x1] = "."
        
        # en-passant
        if piece.lower() == "p" and x1 != x2 and target == ".":
            self.board[y1][x2] = "."
        
        if promotion:
            s = promotion.type_to_string()
            if s == "Queen":
                new_piece = "q" if y1 == 7 else "Q"
            elif s == "Rook":
                new_piece = "r" if y1 == 7 else "R"
            elif s == "Bishop":
                new_piece = "b" if y1 == 7 else "B"
            else:
                new_piece = "n" if y1 == 7 else "N"
            
            self.board[y2][x2] = new_piece
               
        else:
            self.board[y2][x2] = piece
        
        if abs(x1 - x2) == 2:
            if piece == "k":
                # black long castle
                if x2 == 2:
                    self.board[0][3] = "r"
                    self.board[0][0] = "."
                # black short castle    
                else:
                    self.board[0][5] = "r"
                    self.board[0][7] = "."
            elif piece == "K":
                # white long castle
                if x2 == 2:
                    self.board[7][3] = "R"
                    self.board[7][0] = "."
                else:
                    self.board[7][5] = "R"
                    self.board[7][7] = "."
           
             
        fen_board = self.board_to_fen()
        
        
        # checking castling options
        # was white move
        if self.turn == "w":
            if piece == "K":
                self.castling = self.castling.replace('K','').replace('Q', '')
            elif piece == "R":
                if last_move[1] == (7, 7):
                    self.castling = self.castling.replace('K','')
                elif last_move[1] == (0, 7):
                    self.castling = self.castling.replace('Q', '')
        # was black move
        else:
            # also to check if turn complited
            self.fullMove += 1
            if piece == "k":
                self.castling = self.castling.replace('k','').replace('q','')
            elif piece == "r":
                if last_move[1] == (7, 0):
                    self.castling = self.castling.replace('q','')
                elif last_move[1] == (0, 0):
                    self.castling = self.castling.replace('k','')
             
        # if a rook was captured on the starting position     
        if x2 == 0 and y2 == 0:
            self.castling = self.castling.replace('q', '')
        elif x2 == 0 and y2 == 7:
            self.castling = self.castling.replace('Q', '')
        elif x2 == 7 and y2 == 0:
            self.castling = self.castling.replace('k', '')
        elif x2 == 7 and y2 == 7:
            self.castling = self.castling.replace('K', '')
           
        if self.castling == '':
            self.castling = '-'


        self.enPassant = '-'
        # if pawn moved exactly 2 squared last turn
        if piece.lower() == 'p' and abs(y2 - y1) == 2:
            if y1 == 1:
                numEnPassant = 6
            elif y1 == 6:
                numEnPassant = 3
            self.enPassant = f"{chr(x1 + 97)}{numEnPassant}"
        
        if piece.lower() == 'p' or target != ".":
            self.halfMove = 0
        else:
            self.halfMove += 1
            
        # Switch turn
        self.turn = "b" if self.turn == "w" else "w"
        
        self.fen = f"{fen_board} {self.turn} {self.castling} {self.enPassant} {self.halfMove} {self.fullMove}"