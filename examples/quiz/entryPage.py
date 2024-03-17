from PySide6.QtCore import Qt,QIODevice,QCoreApplication
from PySide6.QtWidgets import QWidget, QPushButton, QLabel, QVBoxLayout, QHBoxLayout, QComboBox, QApplication
from PySide6.QtGui import QFont
from PySide6.QtSerialPort import QSerialPort, QSerialPortInfo
import serial.tools.list_ports

from utils.constants import TEXT_COLOR,AMOUNT,QUIT_THRESHOLD
import random
import time

from question import QuizQuestion


class EntryPage(QWidget):

    def __init__(self, main):
        super().__init__()
        self.serial_port = QSerialPort()
        self.action = None
        self.data = None
        self.g_answer = None
        self.main = main
        self.countdown = 0
        
        layout = QVBoxLayout()
        layout.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.setSpacing(0)
        layout.setContentsMargins(0, 0, 0, 0)
        self.setLayout(layout)

        name_label = QLabel("QuizGame")
        name_label.setContentsMargins(0, 0, 0, 50)
        name_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        name_label.setFont(QFont(u"Segoe UI", 45))
        name_label.setStyleSheet(f"color: {TEXT_COLOR};")

        self.category_label = QLabel("No Connection")
        self.category_label.setContentsMargins(0, 0, 0, 10)
        self.category_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.category_label.setFont(QFont(u"Segoe UI", 20))
        self.category_label.setStyleSheet(f"color: {TEXT_COLOR};")

        layout.addWidget(name_label)
        layout.addWidget(self.category_label)

        
        self.connection_label = QLabel("Connection Status: Not Connected")
        self.connection_label.setContentsMargins(0, 0, 0, 10)
        self.connection_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.connection_label.setFont(QFont(u"Segoe UI", 10))
        self.connection_label.setStyleSheet(f"color: {TEXT_COLOR};")
        layout.addWidget(self.connection_label)
        
        self.quit_label = QLabel("")
        self.quit_label.setContentsMargins(0, 0, 0, 10)
        self.quit_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.quit_label.setFont(QFont(u"Segoe UI", 10))
        self.quit_label.setStyleSheet(f"color: {TEXT_COLOR};")
        layout.addWidget(self.quit_label)
        
        self.port_list_dropdown = QComboBox()
        self.refresh_ports()
        self.toggle_port_button = QPushButton("Open Port")
        self.toggle_port_button.clicked.connect(self.toggle_port)

        col_1 = QVBoxLayout()
        col_2 = QVBoxLayout()

        col_1.addWidget(self.port_list_dropdown)
        col_2.addWidget(self.toggle_port_button)
        row = QHBoxLayout()
        row.setSpacing(50)
        row.setContentsMargins(0, 0, 0, 0)
        row.addLayout(col_1)
        row.addLayout(col_2)
        
        layout.addLayout(row)
        
    def refresh_ports(self):
        self.port_list_dropdown.clear()
        ports = serial.tools.list_ports.comports()
        for port in ports:
            self.port_list_dropdown.addItem(port.description, port.device)
            
    def toggle_port(self):
        if self.serial_port.isOpen(): # closed
            self.serial_port.close()
            self.toggle_port_button.setText("Open Port")
            self.connection_label.setText("Connection Status: Not Connected")
            self.category_label.setText("No Connection")
            self.quit_label.setText("")
        else:
            selected_port = self.port_list_dropdown.currentData()
            self.serial_port.setPortName(selected_port)
            self.serial_port.setBaudRate(256000)
            self.serial_port.setDataBits(QSerialPort.Data8)
            self.serial_port.setParity(QSerialPort.NoParity)
            self.serial_port.setStopBits(QSerialPort.OneStop)
            if self.serial_port.open(QIODevice.ReadWrite):
                print(f"Listening on {selected_port}...")
                self.toggle_port_button.setText("Close Port")
                self.connection_label.setText("Connection Status: Connected")
                self.category_label.setText("Detected No Action, Stretch to begin")
                self.quit_label.setText(f"Wait {int(QUIT_THRESHOLD - self.countdown)} times no action to quit...")
                self.serial_port.readyRead.connect(self.read_data)

    def read_data(self):
        try:
            data = str(self.serial_port.readLine(), "utf-8").strip()
            if data[-1] == ",":
                print(f"Velocity: {data}")  
            elif data.startswith("[INFO]"):
                action = data[6:]
                print(f"Received: {data}")
                if not self.action:
                    self.action = action
                self.updateLabel()
        except UnicodeDecodeError:
            pass # ignore
        except Exception as e:
            print(f"Serial port error: {e}")
            
            
    def updateLabel(self):
        if self.main.stacked_widgets.currentIndex() == 0:
            if not self.serial_port.isOpen():
                self.toggle_port()
            self.category_label.setText(f"Detected {self.action}, Stretch to begin")
            # QApplication.processEvents()  # force refresh
            if self.action == "Sit":
                self.wait_quit()
            # else:
            elif self.action == "Stretch":
                self.countdown = 0
                self.quit_label.setText(f"Wait {int(QUIT_THRESHOLD - self.countdown)} times no action to quit...")
                self.action = None
                self.choose_category()
                
            self.action = None
        # elif self.main.stacked_widgets.currentIndex() == 1:
            
                # self.main.question_page.update_action(self.action)
        elif self.main.stacked_widgets.currentIndex() == 2:
            self.action = None
        #     # self.toggle_port()
        #     ...
    def wait_quit(self):
        if self.countdown < QUIT_THRESHOLD-1:
            self.countdown += 1
            self.quit_label.setText(f"Wait {int(QUIT_THRESHOLD - self.countdown)} times no action to quit...")
        else:
            QCoreApplication.quit()       
    def choose_category(self):
        self.data = QuizQuestion().return_question()
        self.main.stacked_widgets.setCurrentIndex(1)
        self.next_question()
        
    def next_question(self):
        # self.main.action = None
        if self.main.question_page.q_number <= AMOUNT:
            f_question = next(self.data)
            question = f_question[0]
            self.g_answer = f_question[1]
            answers = f_question[2] + [self.g_answer]
            random.shuffle(answers)

            self.main.question_page.question.setText(question)
            self.main.question_page.but_1.setText("A: "+answers[0])
            self.main.question_page.but_2.setText("B: "+answers[1])
            self.main.question_page.but_3.setText("C: "+answers[2])
            self.main.question_page.but_4.setText("D: "+answers[3])
            self.main.question_page.app_name_label.setText(f"Q - {self.main.question_page.q_number}")
            self.main.question_page.wait_for_action()
        else:
            self.main.end_page.timer_quit()
            self.main.stacked_widgets.setCurrentIndex(2)
            self.main.end_page.points_collected.setText(f"Your result: {self.main.question_page.points}/{AMOUNT}")
    
