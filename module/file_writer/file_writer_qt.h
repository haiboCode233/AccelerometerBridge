#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include <ctime>
#include <iostream>
#include <fstream>
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

        void getNowTime();
        void startwriteFile(udp_data &dat);
    private:


};

#endif // FILE_WRITER_H
