

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

#include <stdint.h>
#include <errno.h>
#include <getopt.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "TLC59731.h"

static const char *device = "/dev/spidev0.2";
static uint32_t mode = 0x20;
static uint8_t bits = 8;
static uint32_t speed = 100000;
static uint16_t delay = 0;

static uint8_t default_rx[BUFFER_SIZE] = {0, };

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

#define BUFFER_SIZE_ENC 100

class WorkerThread : public QThread {
public:
    void run() override {
        int file_desc_encoder;
        file_desc_encoder = open("/dev/redriver", O_RDWR);

        int file_desc_button;
        file_desc_button = open("/dev/simple_gpio_device", O_RDWR);
	
	int fd;
	fd = open(device, O_RDWR);
	printf("Otvara se SPI fajl\n");
	if( fd == NULL) printf("NULL JE\n");
	int ret = 0;
	
	uint32_t request;
	request = mode;
	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
	ret = ioctl(fd, SPI_IOC_RD_MODE32, &mode);

	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);

	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	
	uint8_t *rx = default_rx;
	
	struct spi_ioc_transfer tr;
	tr.tx_buf = (unsigned long)LED_buffer;
	tr.rx_buf = (unsigned long)rx;
	tr.len = BUFFER_SIZE;
	tr.delay_usecs = delay;
	tr.speed_hz = speed;
	tr.bits_per_word = bits;
	tr.cs_change = 0;

	initialize_buffer();
	
        char buffer[BUFFER_SIZE_ENC];
        while(1){
            read(file_desc_encoder, buffer, BUFFER_SIZE_ENC);
            CircleWidget::x = 3 * atoi(buffer) + 512;
            CircleWidget::y = 100;//(int) 100 * (sin( 2 * 3.14159 * (CircleWidget::x + 1)/ 100 )) + 80;

            read(file_desc_button, state, 2);

		setLED(1, 255, 255, 255);
		setLED(2, 255, 0, 0);
		setLED(3, 0, 255, 0);
		setLED(4, 0, 0, 255);
		setLED(5, 0, 0, 0);
		setLED(6, 255, 255, 0);
		setLED(7, 255, 0, 255);
		setLED(8, 0, 255, 255);
		ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);

            usleep(20000);
        }   
        close(fd);
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
