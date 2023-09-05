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
#include "module/udp/udp_data.h"


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
    void startwriteFile(udp_data &dat);

};


#endif // FILE_WRITER_QT_H
