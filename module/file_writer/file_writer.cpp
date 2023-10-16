#include <file_writer.h>
#include <mainwindow.h>

file_writer::file_writer()
{
    this->outputfrequency = 20;
}

file_writer::~file_writer()
{

}

void file_writer::autoFileName()
{
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(this->time_buffer, 80, "%Y-%m-%d-%H.csv", timeinfo);
    this->outPutFileName = this->time_buffer;
    //std::cout << "Formatted time: " << this->outPutFileName << std::endl;
}

void file_writer::startwriteFile(udp_data &dat)
{
    // 检查文件是否成功打开
    if (!this->outputfile.is_open()) {
        std::cout << "open error\n";
        return;
    }

    static int count = 0;
    for (size_t i = 0; i < 20; i++)
    {
        if (count % (2000/this->outputfrequency) == 0)
        {
            this->outputfile << std::fixed
                             << dat.readFromArray()[0][i] << ","
                             << dat.readFromArray()[1][i] << ","
                             << dat.readFromArray()[2][i] << ","
                             << dat.readFromArray()[3][i] << "\n";
        }
        count++;
    }
    this->outputfile.flush();

}


