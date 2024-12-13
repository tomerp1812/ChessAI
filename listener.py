from abc import ABC, abstractmethod

class Listener(ABC):
    @abstractmethod
    def update():
        pass