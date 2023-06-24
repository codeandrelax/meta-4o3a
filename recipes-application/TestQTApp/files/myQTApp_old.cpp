

#include <iostream>
#include <QApplication>
#include <QWidget>
#include <QLabel>

int main(int argc, char* argv[]){

    QApplication app(argc, argv);

    QLabel hello("<center>Hello, World! Pozdrav od Damjana</center>");
    hello.setWindowTitle("My First QT Application");
    hello.resize(100, 100);
    hello.show();

    return app.exec();
}
