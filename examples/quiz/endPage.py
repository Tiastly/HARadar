from PySide6.QtCore import Qt, QSize, QCoreApplication,QTimer
from PySide6.QtWidgets import QWidget, QPushButton, QLabel, QVBoxLayout
from PySide6.QtGui import QFont

from utils.constants import AMOUNT, TEXT_COLOR, END_BUTTON

class EndPage(QWidget):

    def __init__(self, main):

        super().__init__()
        self.main = main
        
        layout = QVBoxLayout()
        self.setLayout(layout)
        
        labels = QVBoxLayout()
        self.points_collected = QLabel(f"Your result: 0/{AMOUNT}")
        self.points_collected.setStyleSheet(f"color: {TEXT_COLOR};")
        self.points_collected.setFont(QFont(u"Segoe UI", 40))
        self.points_collected.setAlignment(Qt.AlignmentFlag.AlignCenter|Qt.AlignmentFlag.AlignBottom)

        labels.addWidget(self.points_collected)
        
        self.wait_quit = QLabel(f"Wait 5s to start new game...")
        self.wait_quit.setContentsMargins(0, 0, 0, 15)
        self.wait_quit.setStyleSheet(f"color: {TEXT_COLOR};")
        self.wait_quit.setFont(QFont(u"Segoe UI", 15))
        self.wait_quit.setAlignment(Qt.AlignmentFlag.AlignCenter|Qt.AlignmentFlag.AlignBottom)
        labels.addWidget(self.wait_quit)
        
        layout.addLayout(labels)
        
        btn = QVBoxLayout()
        btn.setAlignment(Qt.AlignmentFlag.AlignCenter)
        new_game_button = QPushButton("New Game")
        
        new_game_button.setStyleSheet(END_BUTTON)
        new_game_button.pressed.connect(self.new_game)
        
        exit_button = QPushButton("Exit")
        exit_button.setStyleSheet(END_BUTTON)
        exit_button.pressed.connect(lambda: QCoreApplication.quit())
        
        btn.addWidget(new_game_button)
        btn.addWidget(exit_button)

        layout.addLayout(btn)
        
    def timer_quit(self):
        self.quit = 5
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.updateLabel)
        self.timer.start(1000)
        
    def updateLabel(self):

        self.quit -= 1
        print(f"Wait to begin: {self.quit}")
        if self.quit == 0:
            self.quit = 5
            self.timer.stop()
            self.new_game()
            # QCoreApplication.quit()
            
        self.wait_quit.setText(f"Wait {self.quit} to start new game...")
        
    def new_game(self):
        
        self.wait_quit.setText(f"Wait 5s to start new game...")
        self.main.question_page.q_number = 1
        self.main.question_page.points = 0
        self.main.ent_page.timer.start(500)
        self.main.stacked_widgets.setCurrentIndex(0)