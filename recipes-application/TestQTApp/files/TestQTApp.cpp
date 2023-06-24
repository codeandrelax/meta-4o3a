

#include <iostream>

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>

#include <QThread>
#include <unistd.h>

#include <cmath>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

char state[2];

class CircleWidget : public QWidget {
public:
    static int x;
    static int y;


    CircleWidget() {
        x = 0;
        y = 0;

        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, QOverload<>::of(&CircleWidget::update));
        timer->start(10);
    }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(Qt::black);
        
        if(state[0])
            painter.setBrush(Qt::blue);
        else
            painter.setBrush(Qt::red);

        QRectF circleRect(x, y, 50, 50);
        painter.drawEllipse(circleRect);
    }
};

int CircleWidget::x = 0;
int CircleWidget::y = 0;

#define BUFFER_SIZE 100

class WorkerThread : public QThread {
public:
    void run() override {
        int file_desc_encoder;
        file_desc_encoder = open("/dev/redriver", O_RDWR);

        int file_desc_button;
        file_desc_button = open("/dev/simple_gpio_device", O_RDWR);

        char buffer[BUFFER_SIZE];
        while(1){
            read(file_desc_encoder, buffer, BUFFER_SIZE);
            CircleWidget::x = 3 * atoi(buffer) + 512;
            CircleWidget::y = 100;//(int) 100 * (sin( 2 * 3.14159 * (CircleWidget::x + 1)/ 100 )) + 80;

            read(file_desc_button, state, 2);

            usleep(20000);
        }   
        close(file_desc_encoder);
        close(file_desc_button);
    }
};

void exitFunction(void){
    std::cout << "Izlazim" << std::endl;
    exit(0);
}

int main(int argc, char* argv[]){

    QApplication app(argc, argv);

    CircleWidget *window = new CircleWidget;

    QLabel hello("<center>Testna QT Aplikacija za 4o3a</center>");
    QPushButton *button1 = new QPushButton("IZLAZ");

    QObject::connect(button1, &QPushButton::clicked, &exitFunction);

    hello.setWindowTitle("4o3a STATION MANAGER");
    hello.resize(400, 400);
    hello.show();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(&hello);
    layout->addWidget(button1);

    window->setLayout(layout);
    window->show();

    WorkerThread workerThread;
    workerThread.start();

    return app.exec();  
}