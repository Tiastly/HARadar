import random
import pygame
from dino_runner.components.obstacles.bird import Bird
from dino_runner.components.obstacles.cactus import Cactus
from dino_runner.utils.constants import BIRD


class ObstacleManager:
    def __init__(self):
        self.obstacles = []

    def update(self, game):
        if len(self.obstacles) == 0:
            if random.randint(0,2) == 0:
               self.obstacles.append(Cactus("SMALL"))
            elif random.randint(0,2) == 1:
               self.obstacles.append(Cactus("LARGE"))        
            elif random.randint(0,2) == 2:
               self.obstacles.append(Bird(BIRD, random.randint(200,330)))
        
        for obstacle in self.obstacles:
            obstacle.update(game.game_speed, self.obstacles)
            
            if game.player.dino_rect.colliderect(obstacle.rect):
                if game.player.dino_jump and game.player.jump_vel >=6:
                    game.death = False
                    self.obstacles.remove(obstacle)
                elif not game.player.shield:
                    pygame.time.delay(1000)
                    game.death_count +=1
                    game.playing = False
                    game.death = True
                    break
                else:
                    self.obstacles.remove(obstacle)

    def draw(self, screen):
        for obstacle in self.obstacles:
            obstacle.draw(screen)
        
    def reset_obstacles(self):
        self.obstacles = []