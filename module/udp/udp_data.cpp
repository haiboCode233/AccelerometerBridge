#include "udp_data.h"

udp_data::udp_data()
{
    create2DArray();
}

udp_data::~udp_data()
{
    delete shareData;
}

void udp_data::create2DArray()
{
    shareData = new float*[DATA_ROWS];
    for (int i = 0; i < DATA_ROWS; ++i)
    {
        shareData[i] = new float[DATA_COLS];
    }
}

void udp_data::writeToArray(float (&incomedata)[DATA_ROWS][DATA_COLS])
{
    std::lock_guard<std::mutex> lock(data_mtx);
    int row = 0;
    int col = 0;
    for(row=0;row<DATA_ROWS;row++)
    {
        for(col=0;col<DATA_COLS;col++)
        {
            shareData[row][col] = incomedata[row][col];
        }
    }
}

float** udp_data::readFromArray()
{
    std::lock_guard<std::mutex> lock(data_mtx);
    return shareData;
}
