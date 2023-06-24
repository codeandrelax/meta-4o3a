#ifndef FILEREADERTHREAD_H
#define FILEREADERTHREAD_H

#include <QThread>
#include <string>
#include "Constants.h"
#include <sys/stat.h>
class FileReaderThread : public QThread
{
    Q_OBJECT

public:
    FileReaderThread();

protected:
    void run() override;
    void readFromFile(QString filePath, QString& previousValue, int progressBarNumber, QString typeOfObject);
    bool hasFileChanged(const std::string& filename, const time_t* previousTimestamp, int index);
    void checkAndUpdateValues(const QString& basePath, const QStringList& filenames,
                              QStringList& previousValues, const time_t* previousTimestamps,
                              const QString& typeOfObject);

signals:
    void valueChangedButton(int objectNumber, const QString& value);
    void valueChangedEncoder(int objectNumber, const QString& value);

private:
    QString m_basePath;
    QStringList m_buttonFilenames;
    QStringList m_encoderFilenames;
    QString m_previousValueButton[numberOfButtons];
    QString m_previousValueEncoder[numberOfEncoders];
    time_t m_previousTimestampButton[numberOfButtons];
    time_t m_previousTimestampEncoder[numberOfEncoders];
};

#endif // FILEREADERTHREAD_H
