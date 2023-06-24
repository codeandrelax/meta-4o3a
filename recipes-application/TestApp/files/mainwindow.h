#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "FileReaderThread.h"  // Include the FileReaderThread header file
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onvalueChangedButton(int objectNumber, const QString& value);
    void onvalueChangedEncoder(int objectNumber, const QString& value);
private:
    Ui::MainWindow *ui;
    FileReaderThread m_fileReaderThread;
};

#endif // MAINWINDOW_H
