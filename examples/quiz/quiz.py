from PySide6.QtCore import Qt, QSize, QTimer, QEventLoop
from PySide6.QtWidgets import QWidget, QMainWindow, QPushButton, QLabel, QGridLayout, QStackedWidget,QVBoxLayout, QHBoxLayout, QButtonGroup,QApplication
from PySide6.QtGui import QFont, QColor, QPalette

from entryPage import EntryPage
from endPage import EndPage
from utils.constants import AMOUNT, TEXT_COLOR, BUTTON_SETUP, BUTTON_SETUP_RIGHT, BUTTON_SETUP_WRONG,QUESTION_LABEL



class Window(QMainWindow):

    def __init__(self,client):
        super().__init__()
        self.setWindowTitle("QuizGame") 
        self.main_widget = QWidget()
        self.setCentralWidget(self.main_widget)
        
        palette = self.palette()
        palette.setBrush(QPalette.Window,QColor("#f0f0f0"))
        self.setPalette(palette)

        layout = QGridLayout()
        layout.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)
        self.main_widget.setLayout(layout)

        self.stacked_widgets = QStackedWidget()
        layout.addWidget(self.stacked_widgets)

        self.ent_page = EntryPage(self)
        self.question_page = QuizPage(self)
        self.end_page = EndPage(self)
        self.stacked_widgets.addWidget(self.ent_page)
        self.stacked_widgets.addWidget(self.question_page)
        self.stacked_widgets.addWidget(self.end_page)

        
        self.client = client
        
class QuizPage(QWidget):

    def __init__(self, main):
        super().__init__()
        
        self.main = main
        self.points = 0
        self.q_number = 1

        layout = QVBoxLayout()
        layout.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.setSpacing(0)
        layout.setContentsMargins(0, 0, 0, 0)
        self.setLayout(layout)
        
        hbox = QHBoxLayout()
        # hbox.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.app_name_label = QLabel(f"Q - {self.q_number}")
        self.app_name_label.setAlignment(Qt.AlignmentFlag.AlignTop|Qt.AlignmentFlag.AlignLeft)
        self.app_name_label.setFont(QFont(u"Segoe UI", 20))
        self.app_name_label.setStyleSheet(f"color: {TEXT_COLOR};")
        hbox.addWidget(self.app_name_label)
        
        self.action = ActionLabel("")
        hbox.addWidget(self.action)
        
        layout.addLayout(hbox)
        
        qustion_label = QVBoxLayout()
        self.question = Question('TEST QUESTION?')
        qustion_label.addWidget(self.question)
        
        
        self.but_1 = CategoryButton('A')
        self.but_2 = CategoryButton('C')
        self.but_3 = CategoryButton('B')
        self.but_4 = CategoryButton('D')

        self.button_group = QButtonGroup()
        self.button_group.addButton(self.but_1)
        self.button_group.addButton(self.but_2)
        self.button_group.addButton(self.but_3)
        self.button_group.addButton(self.but_4)

        self.button_group.buttonPressed.connect(self.options)
        
        col1 = QVBoxLayout()
        col2 = QVBoxLayout()

        col1.addWidget(self.but_1)
        col1.addWidget(self.but_3)
        col2.addWidget(self.but_2)
        col2.addWidget(self.but_4)

        row = QHBoxLayout()
        row.setContentsMargins(0, 20, 0, 100)
        row.setSpacing(25)

        row.addLayout(col1)
        row.addLayout(col2)

        qustion_label.addLayout(row)
        layout.addLayout(qustion_label)
        
        tip_label = QLabel("A: Stretch  B: Jump  C: Walk    D: Run")
        tip_label.setAlignment(Qt.AlignmentFlag.AlignTop|Qt.AlignmentFlag.AlignRight)
        tip_label.setFont(QFont(u"Segoe UI", 10))
        tip_label.setStyleSheet(f"color: {TEXT_COLOR};")
        layout.addWidget(tip_label)
        
    def wait_for_action(self):
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_action)
        self.timer.start(2000)
    
    def update_action(self):
        action = self.main.client.getAct()
        if action:
            self.action.setText(action)
            print(f"quizPage: {action}")
            if action == "Stretch":
                self.options(self.but_1)
            elif action == "Jump":
                self.options(self.but_2)
            elif action == "Walk":
                self.options(self.but_3)
            elif action == "Run":
                self.options(self.but_4)
            else:
                self.main.client.setAct(None)
    def options(self, button):
        self.main.client.setAct(None)
        if button.text() == self.main.ent_page.g_answer:
            self.points += 1
        else:
            button.setStyleSheet(BUTTON_SETUP_WRONG)

        correct = None
        if self.but_1.text() == self.main.ent_page.g_answer:
            correct = self.but_1

        elif self.but_2.text() == self.main.ent_page.g_answer:
            correct = self.but_2

        elif self.but_3.text() == self.main.ent_page.g_answer:
            correct = self.but_3

        elif self.but_4.text() == self.main.ent_page.g_answer:
            correct = self.but_4

        self.app_name_label.setText(f"Q - {self.q_number}")
        if self.q_number <= AMOUNT:
            self.q_number += 1
        
        correct.setStyleSheet(BUTTON_SETUP_RIGHT)
        self.but_1.setDisabled(True)
        self.but_2.setDisabled(True)
        self.but_3.setDisabled(True)
        self.but_4.setDisabled(True)
        
        timer = QTimer()
        timer.setSingleShot(True)
        timer.timeout.connect(lambda: self.unfreeze(correct, button))
        timer.start(3000)
        eventloop = QEventLoop()
        eventloop.exec()


    def unfreeze(self, correct, wrong):
        correct.setStyleSheet(BUTTON_SETUP)
        wrong.setStyleSheet(BUTTON_SETUP)
        self.main.ent_page.next_question()
        self.but_1.setDisabled(False)
        self.but_2.setDisabled(False)
        self.but_3.setDisabled(False)
        self.but_4.setDisabled(False)
        self.action.setText(None)
        
class CategoryButton(QPushButton):

    def __init__(self, text: str):
        super().__init__()

        self.label = QLabel(text, self)
        self.label.setWordWrap(True)
        self.label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.label.setStyleSheet(f"color: {TEXT_COLOR};")
        self.label.setFont(QFont(u"Segoe UI", 14))
        
        layout = QHBoxLayout()
        layout.addWidget(self.label)
        self.setLayout(layout)

        self.setFixedSize(QSize(100, 50))
        self.setStyleSheet(BUTTON_SETUP)

    def setText(self, text):
        self.label.setText(text)

    def text(self):
        return self.label.text()[3:]


class Question(QLabel):
    def __init__(self, text):
        super().__init__(text)
        self.setWordWrap(True)
        self.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.setFont(QFont(u"Segoe UI", 20))
        self.setStyleSheet(QUESTION_LABEL)
        self.setText(text)

class ActionLabel(QLabel):
    def __init__(self, text):
        super().__init__(text)
        self.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.setFont(QFont(u"Segoe UI", 20))
        self.setStyleSheet(f"color:{TEXT_COLOR}")

    def setText(self, text):
        super().setText(text)