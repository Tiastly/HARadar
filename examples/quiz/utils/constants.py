TEXT_COLOR = "#34495e"
BG_BUTTON_COLOR = "#ecf0f1"
BORDER_COLOR = "#0078d7"
Q_BORDER_COLOR = "#1565c0"
WRONG_ANSWER_COLOR = "#e74c3c"
GOOD_ANSWER_COLOR = "#2ecc71"

BUTTON_SETUP = f"""
QPushButton {{
    background-color: {BG_BUTTON_COLOR};
    border-style: outset;
    border-width: 4px;
    border-radius: 25px;
    border-color: {BORDER_COLOR};
    font: bold 20px;
    min-width: 10em;
    padding: 6px;
    color: {TEXT_COLOR};
}}

QPushButton:hover {{
    background-color: lightblue;
}}
"""
END_BUTTON = '''
        QPushButton {
            background-color: #6272a4;
            border: none; 
            color: #FFFFFF;
            padding: 10px 20px;
            font-size: 16px;
            border-radius: 5px;
        }
        QPushButton:hover {
            background-color: #2C323D;
        }
    '''

QUESTION_LABEL = f"""
QLabel {{
    background-color: {BG_BUTTON_COLOR};
    border-style: outset;
    border-width: 4px;
    border-radius: 25px;
    border-color: {BORDER_COLOR};
    min-width: 10em;
    padding: 20px;
    color: {TEXT_COLOR};
}}
"""

BUTTON_SETUP_RIGHT = BUTTON_SETUP.replace(BG_BUTTON_COLOR, GOOD_ANSWER_COLOR)
BUTTON_SETUP_WRONG = BUTTON_SETUP.replace(BG_BUTTON_COLOR, WRONG_ANSWER_COLOR)
QUIT_THRESHOLD = 10
AMOUNT = 3