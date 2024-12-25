from players.player import Player
import socket

class Ai(Player):
    client_socket = None
    
    @classmethod
    def initSocket(cls):
        if cls.client_socket is None:
            cls.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            cls.client_socket.connect(("127.0.0.1", 8080))
            
    def __init__(self):
        self.__class__.initSocket()
        super().__init__()
        
    def me(self):
        return "Ai"
    
    def moveStringToMove(self, moveString):
        start = moveString[:2]
        target = moveString[2:4]
        
        
        
        colStart = ord(start[0]) - ord('a')
        rowStart = 8 - int(start[1])
        
        
        colTarget = ord(target[0]) - ord('a')
        rowTarget = 8 - int(target[1])
        
        # promotion
        if len(moveString) == 5:
            return (colStart, rowStart), (colTarget, rowTarget), moveString[4]
        return (colStart, rowStart), (colTarget, rowTarget)
    
    def move(self, fenNotation):
        ans = self.communicate(fenNotation)
        return ans
    
    def communicate(self, fenNotation):
        print(fenNotation)
        data_size = len(fenNotation)

        self.__class__.client_socket.sendall(data_size.to_bytes(4, 'big'))
        self.__class__.client_socket.sendall(fenNotation.encode('utf-8'))
        move = self.__class__.client_socket.recv(200).decode('utf-8')
        
        return self.moveStringToMove(move)




# data_size = len("Done!")
# client_socket.sendall(data_size.to_bytes(4, 'big'))

# Send the actual message
# client_socket.sendall("Done!".encode('utf-8'))

# move = client_socket.recv(200)
# print(move.decode('utf-8')) 
# client_socket.close()
   