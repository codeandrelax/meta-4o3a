#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "FileReaderThread.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&m_fileReaderThread, &FileReaderThread::valueChangedButton, this, &MainWindow::onvalueChangedButton);
    connect(&m_fileReaderThread, &FileReaderThread::valueChangedEncoder, this, &MainWindow::onvalueChangedEncoder);

    m_fileReaderThread.start();
}

MainWindow::~MainWindow()
{
    m_fileReaderThread.quit();
    m_fileReaderThread.wait();

    delete ui;
}

void MainWindow::onvalueChangedButton(int objectNumber, const QString& value)
{
    if (objectNumber == 1)
    {
        if (value == "0")
        {
            ui->pushButton1->setStyleSheet("background-color: blue;");
        }
        else if (value == "1")
        {
            ui->pushButton1->setStyleSheet("background-color: red;");
        }
    }
    if (objectNumber == 2)
    {
        if (value == "0")
        {
            ui->pushButton2->setStyleSheet("background-color: blue;");
        }
        else if (value == "1")
        {
            ui->pushButton2->setStyleSheet("background-color: red;");
        }
    }
}
void MainWindow::onvalueChangedEncoder(int objectNumber, const QString& value)
{
    if (objectNumber == 1)
    {
        if (value.toInt() < 0)
        {
            ui->progressBar1->setValue(ui->progressBar1->value() - 1); // Decrease by 1
        }
        else if (value.toInt() > 0)
        {
            ui->progressBar1->setValue(ui->progressBar1->value() + 1); // Increase by 1
        }
    }
    if (objectNumber == 2)
    {
        if (value.toInt() < 0)
        {
            ui->progressBar2->setValue(ui->progressBar2->value() - 1); // Decrease by 1
        }
        else if (value.toInt() > 0)
        {
            ui->progressBar2->setValue(ui->progressBar2->value() + 1); // Increase by 1
        }
    }
}
