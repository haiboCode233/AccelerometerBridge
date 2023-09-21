#include "module/api/api.h"

acqlib_api::acqlib_api()
{
    ;
}

acqlib_api::~acqlib_api()
{
    ;
}

int acqlib_api::acqlib_init()
{
    // 接收对象
    receiver = new Udp_Receiver_Qt();
    writer = new file_writer();

    return 0;
}


int acqlib_api::acqlib_deinit()
{
    delete receiver;
    delete writer;
    return 0;
}

float acqlib_api::acqlib_get_data(int row, int col)
{
    return receiver->udpOutData.readFromArray()[row][col];
}

int acqlib_api::acqlib_write_file()
{
    writer->startwriteFile(receiver->udpOutData);
    return 0;
}
