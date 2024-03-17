import sys
from quiz import Window, QApplication

if __name__ == "__main__":
    app = QApplication()
    window = Window()
    window.resize(800, 600)
    window.show()
    sys.exit(app.exec()) 