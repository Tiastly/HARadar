from PySide6.QtCore import Qt, QTimer,QCoreApplication
from PySide6.QtWidgets import QWidget, QPushButton, QLabel, QVBoxLayout, QHBoxLayout, QButtonGroup
from PySide6.QtGui import QFont
import random
from utils.constants import TEXT_COLOR,AMOUNT,QUIT_THRESHOLD

from question import QuizQuestion


class EntryPage(QWidget):

    def __init__(self, main):
        super().__init__()

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

        self.category_label = QLabel("Detected No Action, Stretch to begin")
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
        
        self.quit_label = QLabel(f"Wait {int(QUIT_THRESHOLD - self.countdown)} times no action to quit...")
        self.quit_label.setContentsMargins(0, 0, 0, 10)
        self.quit_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.quit_label.setFont(QFont(u"Segoe UI", 10))
        self.quit_label.setStyleSheet(f"color: {TEXT_COLOR};")
        
        col_1 = QVBoxLayout()
        col_2 = QVBoxLayout()

        col_1.addWidget(self.quit_label)

        row = QHBoxLayout()
        row.setSpacing(50)
        row.setContentsMargins(0, 0, 0, 0)
        row.addLayout(col_1)
        row.addLayout(col_2)
        
        layout.addLayout(row)
        self.begin_game()

    def begin_game(self):
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.updateLabel)
        self.timer.start(1000)

    def updateLabel(self):
        action = self.main.client.getAct()
        if self.main.client.isConnected:
            self.connection_label.setText("Connection Status: Connected")
            if action:
                print(f"entryPage: {action}")
                self.category_label.setText(f"Detected {action}, Stretch to begin")
                self.main.client.setAct(None)
                if action == "Sit":
                    self.wait_quit()
                elif action == "Stretch":
                    self.main.client.setAct(None)   
                    self.timer.stop()
                    self.countdown = 0
                    self.quit_label.setText(f"Wait {int(QUIT_THRESHOLD - self.countdown)} times no action to quit...")
                    self.choose_category()
                    
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
    
