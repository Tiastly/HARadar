from dino_runner.components.power_ups.powerup import PowerUp
from dino_runner.utils.constants import HAMMER, HAMMER_TYPE


class Hammer(PowerUp):
    def __init__(self):
        self.image = HAMMER
        self.type = HAMMER_TYPE
        self.index = 0
        super(Hammer, self).__init__(self.image, self.type)