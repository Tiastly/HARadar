import sys
import time
from quiz import Window,QApplication
from webClient import WebSocketClient

if __name__ == "__main__":
    host = "192.168.1.111"
    app = QApplication()
    client = WebSocketClient(host)
    client.init_websocket()
    time.sleep(2)
    window = Window(client)
    window.resize(800, 600)
    window.show()
    sys.exit(app.exec())