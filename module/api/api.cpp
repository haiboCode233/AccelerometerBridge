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
    return 0;
}


int acqlib_api::acqlib_deinit()
{
    return 0;
}

float acqlib_api::acqlib_get_data(int row, int col)
{
    return receiver.udpOutData.readFromArray()[row][col];
}

int acqlib_api::acqlib_write_file()
{
    writer.startwriteFile(receiver.udpOutData);
    return 0;
}

int acqlib_api::acqlib_active_receiver_thread()
{
//    std::thread thread_receiver(&Udp_Receiver::loopReceive, this->receiver);
//    thread_receiver.detach();
    return 0;
}

int acqlib_api::acqlib_start_receive()
{
    receiver.startReceive();
    return 0;
}

int acqlib_api::acqlib_stop_receive()
{
    receiver.stopReceive();
    return 0;
}

