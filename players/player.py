from abc import ABC, abstractmethod

class Player(ABC):
    
    @abstractmethod
    def me(self):
        pass
    