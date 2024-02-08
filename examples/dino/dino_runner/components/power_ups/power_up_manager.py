import random
import pygame
from dino_runner.components.power_ups.shield import Shield


class PowerUpManager:
    def __init__(self):
        self.power_ups = []
        self.when_appers = 0
        self.points = 0
        self.options_number = list(range(1, 10))

    def reset_power_ups (self, points):
        self.power_ups = []
        self.points = points
        self.when_appers = random.randint(200, 300) + points

    def generate_power_ups (self, points, game):
        self.points = points
        # print("==>"+str(self.when_appers))
        if game.player.shield == True and self.when_appers <= self.points:
            self.when_appers+=200
        if len(self.power_ups) == 0:
            if self.when_appers == self.points:
                print("generating powerup")
                self.power_ups.append(Shield())
                self.when_appers = random.randint(self.when_appers + 200, 500 + self.when_appers)
        return self.power_ups

    def update (self, points, game_speed, player, game):
        self.generate_power_ups(points, game)
        for power_up in self.power_ups:
            power_up.update(game_speed, self.power_ups)
            if player.dino_rect.colliderect(power_up.rect):
                power_up.start_time = pygame.time.get_ticks()
                player.shield = True
                player.show_text = True
                player.type = power_up.type
                time_random = random.randrange(5, 8)
                player.shield_time_up = power_up.start_time + (time_random * 1000)
                self.power_ups.remove(power_up)

    def draw (self, screen):
        for power_up in self.power_ups:
            power_up.draw(screen)