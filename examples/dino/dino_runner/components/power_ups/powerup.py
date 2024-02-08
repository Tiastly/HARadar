import random
from pygame.sprite import Sprite
from dino_runner.utils.constants import SCREEN_WIDTH
class PowerUp(Sprite):
    def __init__(self, image, type):
        self.image = image
        self.type = type
        self.rect = self.image.get_rect()
        self.rect.x = SCREEN_WIDTH + random.randint(100, 150)
        self.rect.y = random.randint(100, 150)
        self.start_time = 0
        self.width = self.image.get_width()

    def update(self, game_speed, powerups):
        self.rect.x -= game_speed 
        if self.rect.x < -self.rect.width:
            powerups.pop()

    def draw(self, screen):
        screen.blit(self.image, self.rect)