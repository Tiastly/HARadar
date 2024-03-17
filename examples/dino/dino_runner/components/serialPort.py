import serial
import threading
import time
class SerialPort:
    def __init__(self, port, game, baudrate=256000):
        self.port = port
        self.baudrate = baudrate
        self.serial = None
        self.game = game
        self.stop_flag = threading.Event()
    def connect(self):
        try:
            self.serial = serial.Serial(self.port, self.baudrate)
            self.stop_flag.clear()
            self.thread = threading.Thread(target=self.listen)
            self.thread.start()
            print(f"Listening on {self.port}...")
        except serial.SerialException as e:
            print(f"connect error: {e}")
            self.reconnecting()
            
    def reconnecting(self):
        self.reconnect = True
        while self.reconnect:
            try:
                self.serial = serial.Serial(self.port, self.baudrate)
                self.reconnect = False
                self.stop_flag.clear()
                if not hasattr(self, 'thread') or not self.thread.is_alive():
                    self.thread = threading.Thread(target=self.listen)
                    self.thread.start()
                print(f"Reconnected to {self.port}...")
            except serial.SerialException as e:
                print(f"reconnect error: {e}")
                time.sleep(2)
                
    def listen(self):
        while not self.stop_flag.is_set():
            try:
                if not self.game.playing: # quit
                    self.close() 
                if self.serial.in_waiting > 0:
                    self.game.is_connected = True
                    data = str(self.serial.readline(), "utf-8").strip()
                    if data[-1] == ",":
                        print(f"Velocity: {data}")  
                    elif data.startswith("[INFO]"):
                        action = data[6:]
                        print(f"Received: {data}")
                        self.game.setActions(action)
                        
            except UnicodeDecodeError:
                continue # ignore
            except Exception as e:
                print(f"serial error: {e}")
                self.game.is_connected = False
                self.reconnecting()
        else:
            self.serial.close()
            print("Serial port closed")
            
    def on_message(self, activity):
        print(activity)
        self.game.setActions(activity)

    def close(self):
        self.game.is_connected = False
        self.stop_flag.set()
        

if __name__ == "__main__":
    port = "COM8" 
    baudrate = 256000 
    listener = SerialPort(port, game=None)
    listener.start()
