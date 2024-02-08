# from tkinter import END
import pygame
import time
import asyncio
from dino_runner.components import text_utils
from dino_runner.components.cloud import Cloud
from dino_runner.components.dino import Dino
from dino_runner.components.obstacles.obstaclemanager import ObstacleManager
from dino_runner.components.power_ups.power_up_manager import PowerUpManager

from dino_runner.components.websocketclient import WebSocketClient
from dino_runner.utils.constants import BG, DINO_DEAD, DINO_START, GAME_OVER, ICON, RESET, SCREEN_HEIGHT, SCREEN_WIDTH, TITLE, FPS


ACTION_MAP = {
    "Stretch": "begin", 
    "Jump": "Jump",
    "Walk": "Walk",
    "Run": "Run",
    "Sit": "quit"}

QUIT_THRESHOLD = 10
class Game:
    COLORS = [
        (255, 255, 255), #day
        (0, 0, 0)# night
    ]
    def __init__(self):
        pygame.init()
        pygame.display.set_caption(TITLE)
        pygame.display.set_icon(ICON)
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        self.clock = pygame.time.Clock()
        self.playing = False
        # self.running = True
        self.game_speed = 0
        self.x_pos_bg = 0
        self.y_pos_bg = 380
        self.cloud = Cloud()
        self.player = Dino()
        self.obstacle_manager = ObstacleManager()
        self.points = 0
        self.death_count = 0
        self.power_up_manager = PowerUpManager()
        self.high_score = 0
        self.bg_color = 0
        self.death = False 
        self.last_gesture = None
        self.new_action = True
        self.countdown = 0
        self.loop_count = 0
        self.is_connected = False
        
    def getActions(self):
        if self.new_action:
            self.new_action = False
            return self.last_gesture
        else:
            return None
        
    def setActions(self,action):
        #connect with websocket
        self.last_gesture = action
        self.new_action = True
        
    def execute(self):
        # while self.running:
        while True:
            if not self.playing:
                self.show_menu()
                self.quit_events()
                self.begin_game(self.getActions()) 
                
    def show_menu(self):
        self.screen.fill((255,255,255))
        if not self.is_connected:
            self.check_connection()
        else:
            self.print_menu_elements()
        pygame.display.update()
        
    def check_connection(self):
        if not self.is_connected:
            start, start_rect = text_utils.get_centered_message("Waiting for connection...")
            self.screen.blit(start, (start_rect.x,270))
            return False
        return True
    def print_menu_elements(self):
        if self.death_count == 0:
            start, start_rect = text_utils.get_centered_message("STRETCH TO START")
            self.screen.blit(start, (start_rect.x,270))
            self.screen.blit(DINO_START, (80, 310))
            quit, quit_rect = text_utils.get_centered_message(f"Wait {int(QUIT_THRESHOLD - self.countdown)} times no action to quit...",font_size=25)
            self.screen.blit(quit, (quit_rect.x, 350))
        else:
            if self.points > self.high_score:
                self.high_score = self.points
            text_death, text_death_rect = text_utils.get_centered_message(f" Deaths:  {self.death_count}",font_size=16)
            text_points, text_points_rect = text_utils.get_centered_message(f" Points: {self.points}",font_size=16)
            text_round, text_round_rect = text_utils.get_centered_message(f" Round: {self.death_count}",font_size=16)
            text_hi, text_hi_rect = text_utils.get_centered_message(f" HI: {self.high_score}",font_size=16)
            text, text_rect = text_utils.get_centered_message("STRETCH TO START AGAIN")
            quit, quit_rect = text_utils.get_centered_message(f"Wait {int(QUIT_THRESHOLD - self.countdown)} times no action to quit...",font_size=20)
            self.death = False
            self.screen.blit(text_death, (text_death_rect.x, 150))
            self.screen.blit(text_points, (text_points_rect.x, 180 ))
            self.screen.blit(text_round, (text_round_rect.x, 210))
            self.screen.blit(text_hi, (text_hi_rect.x, 240))
            self.screen.blit(text, (text_rect.x, 300))
            self.screen.blit(quit, (quit_rect.x, 350))
            self.screen.blit(DINO_DEAD, (80, 310)) 
            # self.screen.blit(RESET, (520, 350))
            self.screen.blit(GAME_OVER, (350, 100))
        pygame.display.update()

    def begin_game(self,action):
        if not self.death:  #restart game
            if action == "Stretch":
                self.countdown = 0
                self.player.__init__()
                # self.action_count = 0
                self.run()
            # elif action != None:
            elif action == "Sit":
                self.wait_quit()
    
    def run(self):
        self.START_TIME = time.time()
        self.create_components()
        self.playing = True
        self.points = 0
        # self.game_speed = 20
        while self.playing:
            self.quit_events()
            if not self.is_connected:
                self.check_connection()
            else:
                self.update(self.getActions())
                self.draw()
            
    
    def update(self,action):
        if action:
            self.loop_count += 1
            if action in ["Jump","Walk","Run"]:
                self.game_speed = 20
                
        else: #no action 
            if self.player.dino_jump == False and self.player.dino_run == False and self.player.dino_walk == False:
                self.game_speed = 0

        self.player.update(action)
        self.cloud.update(self)  # cloud speed
        self.obstacle_manager.update(self) # random obstacles
        self.power_up_manager.update(self.points, self.game_speed, self.player, self)#shield

    def draw(self):
        if self.loop_count > QUIT_THRESHOLD: #change background color every 10 times (10secs)
            self.bg_color = (self.bg_color + 1) % len(self.COLORS)
            self.loop_count = 0
        
        self.clock.tick(FPS)
        self.screen.fill(self.COLORS[self.bg_color])
        self.draw_background()
        self.cloud.draw(self.screen)
        self.player.draw(self.screen)
        self.power_up_manager.draw(self.screen)
        self.score()
        self.obstacle_manager.draw(self.screen)
        pygame.display.update()
        # pygame.display.flip()
        

    def draw_background(self):
        image_width = BG.get_width()
        self.screen.blit(BG, (self.x_pos_bg, self.y_pos_bg))
        self.screen.blit(BG, (image_width + self.x_pos_bg, self.y_pos_bg))
        if self.x_pos_bg <= -image_width:
            self.screen.blit(BG, (image_width + self.x_pos_bg, self.y_pos_bg))
            self.x_pos_bg = 0
        self.x_pos_bg -= self.game_speed

    def score(self):
        if self.game_speed > 0:
            self.points += 1
        text, text_rect = text_utils.get_score_element("Points: ",self.points, color=self.bg_color)  
        self.screen.blit(text, text_rect)
        text, text_rect = text_utils.get_score_element("HI: ", self.high_score, 900, color=self.bg_color)  
        self.screen.blit(text, text_rect)
        text, text_rect = text_utils.get_score_element("Action: ",self.last_gesture,800, color=self.bg_color) 
        self.screen.blit(text, text_rect)
        
        self.player.check_invincibility(self.screen)
        
    def create_components(self):
        self.obstacle_manager.reset_obstacles()
        self.power_up_manager.reset_power_ups(self.points)
        
    def wait_quit(self):
        if self.countdown < QUIT_THRESHOLD-1:
            self.countdown += 1
        else:
            self.playing = False
            self.death = False
            pygame.display.quit()
            pygame.quit()
            exit()
            
    def quit_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                # self.running = False
                self.playing = False
                self.death = False
                pygame.display.quit()
                pygame.quit()
                exit()