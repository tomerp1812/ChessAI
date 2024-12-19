from player import Player
import socket
import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from fen import Fen


class Ai(Player):
    def __init__(self, fen):
        self.fen = fen
        super().__init__()
        
    def me(self):
        return "Ai"
    
    # def fenNotation(self, state):
    #     pass
    
    def move(self, state):
        fenNotation = self.fen.getFen()
        ans = self.communicate(state)
        pass
    
    def communicate(self, state):
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect(("127.0.0.1", 8080))
        
        for i in range(10):
            data_size = len(self.fen.createStartingPosition())
            client_socket.sendall(data_size.to_bytes(4, 'big'))

            # Send the actual message
            client_socket.sendall(self.fen.createStartingPosition().encode('utf-8'))

            move = client_socket.recv(200)
            print(move.decode('utf-8'))
            
           
        data_size = len("Done!")
        client_socket.sendall(data_size.to_bytes(4, 'big'))

        # Send the actual message
        client_socket.sendall("Done!".encode('utf-8'))

        move = client_socket.recv(200)
        print(move.decode('utf-8')) 
        client_socket.close()
   
   
fen = Fen()
ai = Ai(fen)
ai.move("Dosdne!")