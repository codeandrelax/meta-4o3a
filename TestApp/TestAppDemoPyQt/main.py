import sys
from PyQt5.QtWidgets import QApplication
from TestApp import TestApp
import argparse

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--num_buttons', type=int, default=20,
                        help='number of buttons to create')
    parser.add_argument('--num_scroll_handlers', type=int, default=4,
                        help='number of scroll handlers to create')
    args = parser.parse_args()

    app = QApplication(sys.argv)
    ex = TestApp(num_buttons=args.num_buttons, num_scroll_handlers=args.num_scroll_handlers)
    sys.exit(app.exec_())