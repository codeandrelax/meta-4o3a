#include "FileReaderThread.h"
#include "FileReaderThread.h"
#include "Constants.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>


void FileReaderThread::readFromFile(QString filePath, QString& previousValue, int progressBarNumber, QString typeOfObject)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        QString value = in.readAll().trimmed();
        file.close();
        qDebug() <<"this is prevous " <<previousValue<<"  this is current  "<< value;
        if (value != previousValue)  // Check if the value has changed
        {
            if (typeOfObject == "Button")
            {
                emit valueChangedButton(progressBarNumber, value);
            }
            else if (typeOfObject == "Encoder")
            {
                emit valueChangedEncoder(progressBarNumber, value);
            }
            previousValue = value;  // Update the previous value
        }
    }
}


bool FileReaderThread::hasFileChanged(const std::string& filename, const time_t* previousTimestamp, int index) {
    struct stat fileStat;
    if (stat(filename.c_str(), &fileStat) == 0) {
        const time_t currentTimestamp = fileStat.st_mtime;
        bool isChanged = currentTimestamp > previousTimestamp[index];
        return isChanged;
    }

    return false; // File doesn't exist or error occurred, consider it unchanged
}
