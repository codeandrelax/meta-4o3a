from PyQt5.QtWidgets import QWidget, QVBoxLayout, QProgressBar


class ScrollHandler(QWidget):
    def __init__(self, num_scroll_handlers):
        super().__init__()
        self.title = 'Volume Bar'
        self.width = 200
        self.height = 200
        self.volume = 50  # initial volume

        self.initUI(num_scroll_handlers)

    def initUI(self, num_scroll_handlers):
        self.setWindowTitle(self.title)
        self.setGeometry(100, 100, self.width, self.height)

        # Create volume progress bars for each scroll handler
        self.volume_bars = []
        for i in range(num_scroll_handlers):
            volume_bar = QProgressBar(self)
            volume_bar.setValue(self.volume)
            volume_bar.setTextVisible(False)
            self.volume_bars.append(volume_bar)

        # Add volume bars to layout
        layout = QVBoxLayout()
        for volume_bar in self.volume_bars:
            layout.addWidget(volume_bar)
        self.setLayout(layout)

    def wheelEvent(self, event):
        # Scroll event
        scroll_amount = event.angleDelta().y()
        if scroll_amount > 0:
            self.volume += 5  # increase volume
            if self.volume > 100:
                self.volume = 100
        elif scroll_amount < 0:
            self.volume -= 5  # decrease volume
            if self.volume < 0:
                self.volume = 0

        # Update volume progress bars for all scroll handlers
        for volume_bar in self.volume_bars:
            volume_bar.setValue(self.volume)
