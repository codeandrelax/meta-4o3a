from PyQt5.QtWidgets import QPushButton, QVBoxLayout, QHBoxLayout, QWidget


class ButtonHandler(QWidget):
    def __init__(self, num_buttons):
        super().__init__()
        self.num_buttons = num_buttons
        self.button_list = []
        self.initUI()

    def initUI(self):
        layout = QVBoxLayout()
        layout.setSpacing(40)
        for i in range(self.num_buttons):
            button = QPushButton('Button {}'.format(i+1), self)
            button.setStyleSheet('background-color: blue')
            self.button_list.append(button)

            if i % 10 == 0:
                hlayout = QHBoxLayout()
                hlayout.setSpacing(20)
                layout.addLayout(hlayout)

            hlayout.addWidget(button)

        self.setLayout(layout)

    def handleKeyPress(self, event):
        key = event.text()
        if key.isdigit():
            button_index = int(key)
        else:
            button_index = ord(key.lower()) - 87

        if button_index < self.num_buttons:
            button = self.button_list[button_index]
            if button.styleSheet() == 'background-color: blue':
                button.setStyleSheet('background-color: red')
            else:
                button.setStyleSheet('background-color: blue')
