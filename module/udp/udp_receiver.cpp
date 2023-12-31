#include "udp_receiver.h"

Udp_Receiver::Udp_Receiver()
{
    ;
}

Udp_Receiver::~Udp_Receiver()
{
    ;
}

void Udp_Receiver::loopReceive_callback(std::function<void()> callback)
{
    while(true)
    {
        if(isRunning)
        {
            udpreceiver.startReceive_new(udpOutData);
            callback();
        }
        else
        {
            ;
        }
    }
}

void Udp_Receiver::loopReceive()
{
    while(true)
    {
        if(isRunning)
        {
            udpreceiver.startReceive_new(udpOutData);
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
