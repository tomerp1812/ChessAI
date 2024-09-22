from Ai.randomAi import RandomAi
from Ai.simpleAi import SimpleAi

class Menu:
    def __init__(self, controller):
        self.controller = controller
        self.running = True
        
    def show(self):
        # Set a background color
        self.controller.screen.fill("lightblue")

        # Title and options
        title = "ChessAi"
        choose = "Choose your opponent"
        opp1 = "1. Random Ai"
        opp2 = "2. Simple Ai"
        opp3 = "3. Human"

        box1_color = "black"
        box2_color = "black"
        box3_color = "black"
        # Render the text with updated font sizes
        title_text = self.controller.big_font.render(title, True, "darkred")
        choose_text = self.controller.medium_font.render(choose, True, "darkblue")
        opp1_text = self.controller.font.render(opp1, True, "white")
        opp2_text = self.controller.font.render(opp2, True, "white")
        opp3_text = self.controller.font.render(opp3, True, "white")

        # Display the text on the screen
        self.controller.screen.blit(title_text, (self.controller.screen.get_width() // 2 - title_text.get_width() // 2, 50))
        self.controller.screen.blit(choose_text, (self.controller.screen.get_width() // 2 - choose_text.get_width() // 2, 180))

        # Adjust box sizes and colors for better clarity
        box1 = self.controller.pygame.Rect(self.controller.screen.get_width() // 2 - 120, 282, 240, 60)
        box2 = self.controller.pygame.Rect(self.controller.screen.get_width() // 2 - 120, 352, 240, 60)
        box3 = self.controller.pygame.Rect(self.controller.screen.get_width() // 2 - 120, 422, 240, 60)

        # Draw rectangles for the options with a clearer border
        self.controller.pygame.draw.rect(self.controller.screen, "black", box1, 0)
        self.controller.pygame.draw.rect(self.controller.screen, "black", box2, 0)
        self.controller.pygame.draw.rect(self.controller.screen, "black", box3, 0)

        # Render and center the option texts inside the boxes
        self.controller.screen.blit(opp1_text, (self.controller.screen.get_width() // 2 - opp1_text.get_width() // 2, 300))
        self.controller.screen.blit(opp2_text, (self.controller.screen.get_width() // 2 - opp2_text.get_width() // 2, 370))
        self.controller.screen.blit(opp3_text, (self.controller.screen.get_width() // 2 - opp3_text.get_width() // 2, 440))

        # Draw borders around the option boxes
        self.controller.pygame.draw.rect(self.controller.screen, "darkblue", box1, 3)
        self.controller.pygame.draw.rect(self.controller.screen, "darkblue", box2, 3)
        self.controller.pygame.draw.rect(self.controller.screen, "darkblue", box3, 3)

        self.controller.pygame.display.flip()

        # Event loop
        while self.running:
            mouse_pos = self.controller.pygame.mouse.get_pos()
            if box1.collidepoint(mouse_pos):
                box1_color = "darkblue"
            else:
                box1_color = "black"
            if box2.collidepoint(mouse_pos):
                box2_color = "darkblue"
            else:
                box2_color = "black"
            if box3.collidepoint(mouse_pos):
                box3_color = "darkblue"
            else:
                box3_color = "black"
            for event in self.controller.pygame.event.get():
                if event.type == self.controller.pygame.QUIT:
                    self.running = False
                    return None, False
                if event.type == self.controller.pygame.MOUSEBUTTONDOWN:
                    if box1.collidepoint(mouse_pos):
                        return RandomAi(), False
                    elif box2.collidepoint(mouse_pos):
                        return SimpleAi(), False
                    elif box3.collidepoint(mouse_pos):
                        return None, True
            self.controller.pygame.draw.rect(self.controller.screen, box1_color, box1, 0)
            self.controller.pygame.draw.rect(self.controller.screen, box2_color, box2, 0)
            self.controller.pygame.draw.rect(self.controller.screen, box3_color, box3, 0)

            # Redraw the borders
            self.controller.pygame.draw.rect(self.controller.screen, "darkblue", box1, 3)
            self.controller.pygame.draw.rect(self.controller.screen, "darkblue", box2, 3)
            self.controller.pygame.draw.rect(self.controller.screen, "darkblue", box3, 3)
            self.controller.screen.blit(opp1_text, (self.controller.screen.get_width() // 2 - opp1_text.get_width() // 2, 300))
            self.controller.screen.blit(opp2_text, (self.controller.screen.get_width() // 2 - opp2_text.get_width() // 2, 370))
            self.controller.screen.blit(opp3_text, (self.controller.screen.get_width() // 2 - opp3_text.get_width() // 2, 440))

            self.controller.pygame.display.flip()
            # self.controller.clock.tick(60)
