import random
from dino_runner.utils.constants import CLOUD, SCREEN_WIDTH


class Cloud:
    def __init__(self):
        
      self.x_cloud_pos = SCREEN_WIDTH 
      self.y_cloud_pos = 0
      self.image = CLOUD
      self.width = self.image.get_width()
   
    def update(self,game):
       self.x_cloud_pos -= game.game_speed 
       if self.x_cloud_pos == self.width:
          self.x_cloud_pos = SCREEN_WIDTH 
          self.y_cloud_pos = 0

    def draw(self, screen):
       screen.blit(self.image, (self.x_cloud_pos, 50))      
       screen.blit(self.image, (self.x_cloud_pos - 200, 70))
       screen.blit(self.image, (self.x_cloud_pos + 800, 10))
       screen.blit(self.image, (self.x_cloud_pos + 400, 130))
       screen.blit(self.image, (self.x_cloud_pos + 600, 100))
       screen.blit(self.image, (self.x_cloud_pos + 1000, 180))
       screen.blit(self.image, (self.x_cloud_pos + 1500, 50))
       screen.blit(self.image, (self.x_cloud_pos + 1300, 100))
       self.x_cloud_pos -= 1
       if self.x_cloud_pos < -1100:
          self.x_cloud_pos = SCREEN_WIDTH
    