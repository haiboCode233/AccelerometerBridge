#ifndef FILE_WRITER_QT_H
#define FILE_WRITER_QT_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QDateTime>


#include <iostream>
#include <fstream>
#include <queue>
#include <vector>


class file_writer_qt : public QObject
{
    Q_OBJECT

public:
    file_writer_qt();
    ~file_writer_qt();

    std::ofstream outputfile;
    QString outputfilename;
    int outputfrequency;

public slots:
    void startwriteFile(void* data);

};


#endif // FILE_WRITER_QT_H