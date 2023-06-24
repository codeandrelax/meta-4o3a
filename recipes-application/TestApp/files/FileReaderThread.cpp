// FileReaderThread.cpp
#include "FileReaderThread.h"
#include "Constants.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <iostream>
FileReaderThread::FileReaderThread()
{
}

void FileReaderThread::run()
{
    QString previousValueButton[] { "0","0" };
    QString previousValueEncoder[] { "0","0" };
    time_t previousTimestampButton[numberOfButtons] = { 0, 0 };
    time_t previousTimestampEncoder[numberOfEncoders] = { 0, 0 };

    QString basePath { "/dev/" };

    QString filePathButton = basePath + "smple_gpio_device";//basePath + buttons[i] + ".txt";
    std::string filenameButton = filePathButton.toStdString();
    
    QFile fileButton(filePathButton);

    QString filePathEncoder = basePath + "redriver";//basePath + encoders[i] + ".txt";
    std::string filenameEncoder = filePathEncoder.toStdString();

    QFile fileEncoder(filePathEncoder);

    while (!QThread::currentThread()->isInterruptionRequested())
    {
        
        //for (int i = 0; i < numberOfButtons; i++)
        //{
            
            
	    //std::cout << filename << std::endl;
            //if (hasFileChanged(filename, previousTimestampButton, i))
            //{
                //readFromFile(filePath, previousValueButton[i], (i + 1),"Button");

                QTextStream in(&fileButton);
                QString value = in.readAll().trimmed();
                //qDebug() <<"this is prevous " <<previousValue<<"  this is current  "<< value;

                emit valueChangedButton(0, QString(value[0]));
                emit valueChangedButton(1, QString(value[1]));
            //}
        //}
        //for (int i = 0; i < numberOfEncoders; i++)
        //{
            

            //if (hasFileChanged(filename, previousTimestampEncoder, i))
            //{
                //readFromFile(filePath, previousValueEncoder[i], (i + 1),"Encoder");
                QTextStream inE(&fileEncoder);
                value = inE.readAll().trimmed();
                //qDebug() <<"this is prevous " <<previousValue<<"  this is current  "<< value;

                emit valueChangedEncoder(1, value);
            //}
        //}
        // Sleep for a short duration before reading again
        usleep(500000);
    }

    fileButton.close();
    fileEncoder.close();
}

