from players.player import Player

class Human(Player):
    def __init__(self, controller):
        self.controller = controller
        super().__init__()
        
        
    def me(self):
        return "Human"
    
    
    def get_position(self):
        clicked_position = self.controller.pygame.mouse.get_pos()
        index_x = clicked_position[0] // 100
        index_y = clicked_position[1] // 100
        return (index_x, index_y)
        
        
    def move(self):
        for event in self.controller.pygame.event.get():
            if event.type == self.controller.pygame.QUIT:
                return None, False
            
            if event.type == self.controller.pygame.MOUSEBUTTONDOWN:
                return self.get_position(), True
            
        return None, True