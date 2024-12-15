from abc import ABC, abstractmethod

class Drawable(ABC):
    @abstractmethod
    def draw(self, screen):
        pass