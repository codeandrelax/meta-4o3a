from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QVBoxLayout, QGridLayout
import sys
from ButtonHandler import ButtonHandler
from ScrollHandler import ScrollHandler


class TestApp(QWidget):
    def __init__(self, num_buttons, num_scroll_handlers):
        super().__init__()
        self.num_buttons = num_buttons
        self.num_scroll_handlers = num_scroll_handlers
        self.title = 'PyQt Test App'
        self.left = 200
        self.top = 200
        self.width = 600
        self.height = 400
        self.button_handler = ButtonHandler(self.num_buttons)
        self.scroll_handler = ScrollHandler(self.num_scroll_handlers)
        self.initUI()

    def initUI(self):
        self.setWindowTitle(self.title)
        self.setGeometry(self.left, self.top, self.width, self.height)
        grid_layout = QGridLayout()

        # Add button handler to the top row of the grid
        grid_layout.addWidget(self.button_handler, 0, 0, 1, 3)

        # Add scroll handlers to the remaining rows of the grid
        num_rows = (self.num_scroll_handlers + 2) // 3  # round up to nearest integer
        for row in range(num_rows):
            for col in range(3):
                index = row * 3 + col
                if index < self.num_scroll_handlers:
                    grid_layout.addWidget(self.scroll_handler, row + 1, col)

        self.setLayout(grid_layout)
        self.show()

    def keyPressEvent(self, event):
        self.button_handler.handleKeyPress(event)
