#ifndef UDP_DATA_H
#define UDP_DATA_H
#include <iostream>
#include <mutex>
#define DATA_ROWS 4
#define DATA_COLS 20


class udp_data
{
    public:
        udp_data();
        ~udp_data();
        void create2DArray();
        void writeToArray(float (&incomedata)[DATA_ROWS][DATA_COLS]);
        float** readFromArray();
    private:
        float **shareData;
        std::mutex data_mtx;

};


#endif // UDP_DATA_H
