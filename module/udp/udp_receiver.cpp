#include "udp_receiver.h"

Udp_Receiver::Udp_Receiver()
{
    ;
}

Udp_Receiver::~Udp_Receiver()
{
    ;
}

void Udp_Receiver::loopReceive(std::function<void()> callback)
{
    while(true)
    {
        if(isRunning)
        {
            udpreceiver.startReceive_new(udpOutData);
            receive_finish = true;
            callback();
        }
        else
        {
            ;
        }
    }
}

void Udp_Receiver::startReceive()
{
    isRunning = true;
}

void Udp_Receiver::stopReceive()
{
    isRunning = false;
}

void Udp_Receiver::get_bias()
{
    udpreceiver.getBias();
}
