#include <file_writer_qt.h>
#include <mainwindow.h>

file_writer_qt::file_writer_qt()
{
    this->outputfrequency = 20;
}

file_writer_qt::~file_writer_qt()
{

}

void file_writer_qt::startwriteFile(udp_data &dat)
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


