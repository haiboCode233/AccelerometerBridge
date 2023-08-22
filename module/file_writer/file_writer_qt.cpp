#include <file_writer_qt.h>

file_writer_qt::file_writer_qt()
{
    this->outputfrequency = 20;
}

file_writer_qt::~file_writer_qt()
{

}

void file_writer_qt::startwriteFile(void* data)
{
    float (*floatMatrix)[20] = static_cast<float(*)[20]>(data);


    // 检查文件是否成功打开
    if (!this->outputfile.is_open()) {
        std::cout << "open error\n";
        return;
    }

    static int count = 0;
    for (size_t i = 0; i < 20; i++)
    {
//        QDateTime currentDateTime = QDateTime::currentDateTime();
        if (count % (2000/this->outputfrequency) == 0)
        {
//            this->outputfile << currentDateTime.date().toString(Qt::ISODate).toStdString() << " "
//                             << currentDateTime.time().toString("hh:mm:ss.zzz").toStdString() << ","
//                             << floatMatrix[0][i] << ","
//                             << floatMatrix[1][i] << ","
//                             << floatMatrix[2][i] << ","
//                             << floatMatrix[3][i] << "\n";

            this->outputfile << floatMatrix[0][i] << ","
                             << floatMatrix[1][i] << ","
                             << floatMatrix[2][i] << ","
                             << floatMatrix[3][i] << "\n";
        }
        count++;
    }
    this->outputfile.flush();

}


