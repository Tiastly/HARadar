from dino_runner.components.game import Game
from dino_runner.components.serialPort import SerialPort
if __name__ == "__main__":
    port = "COM8"
    game = Game()
    serial = SerialPort(port,game)
    serial.connect()
    game.execute()
    