#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include <ctime>
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include "module/udp/udp_data.h"


class file_writer
{

public:
    file_writer();
    ~file_writer();

    std::ofstream outputfile;
    std::string outPutFileName;
    char time_buffer[80];
    int outputfrequency;

    void autoFileName();
    void startwriteFile(udp_data &dat);
private:


};


#endif // FILE_WRITER_QT_H
