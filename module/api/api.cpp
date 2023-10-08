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
    acqlib_samp_freq(20);
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

int acqlib_api::acqlib_active_receiver_thread(acqlib_api * api, CallbackFunction callback_func)
{
    std::thread thread_receiver(&Udp_Receiver::loopReceive, &api->receiver, callback_func);
    thread_receiver.detach();
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

int acqlib_api::acqlib_samp_freq(int freq)
{
    if(freq > MAX_SAMP_FREQ)
    {
        freq = MAX_SAMP_FREQ;
    }
    if(freq < MIN_SAMP_FREQ)
    {
        freq = MIN_SAMP_FREQ;
    }
    writer.outputfrequency = freq;
    return 0;
}

