# from dino_runner.components.game import Game
import websocket
import json
import time
import threading

class WebSocketClient:
    def __init__(self, host,game):
        print("Trying to open a WebSocket connection...")
        self.game = game
        self.gateway = f"ws://{host}/ws"
    def on_error(self, ws, error):
        print(f"Error: {error}")

    def on_close(self, ws, close_status_code, close_msg):
        self.game.is_connected = False
        print(f"Connection closed {close_msg} {close_status_code}")
        time.sleep(2)
        self.init_websocket()
        
    def on_open(self, ws):      
        print("Connection opened")
        self.game.is_connected = True
        
    def init_websocket(self):
        print("Trying to open a WebSocket connection...")
        self.ws = websocket.WebSocketApp(self.gateway,
                                    on_message=self.on_message,
                                    on_error=self.on_error,
                                    on_close=self.on_close,
                                    on_open=self.on_open)
        
        ws_thread = threading.Thread(target=self.ws.run_forever)
        ws_thread.daemon = True 
        ws_thread.start()
        
    def on_message(self, ws, message):
        # print(message)
        obj = json.loads(message)[0]
        act = obj["act"]
        # vel = obj["vel"]
        # col_x = obj["col_x"]
        # col_y = obj["col_y"]
        print(f"act: {act}")
        self.game.setActions(act)

if __name__ == "__main__":
    host = "192.168.1.111"
    # game = Game()
    game = None
    client = WebSocketClient(host,game)
    client.init_websocket()
    # game.execute()