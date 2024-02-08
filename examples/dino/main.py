from dino_runner.components.game import Game
from dino_runner.components.websocketclient import WebSocketClient
if __name__ == "__main__":
    host = "192.168.1.111"
    game = Game()
    client = WebSocketClient(host,game)
    client.init_websocket()
    game.execute()