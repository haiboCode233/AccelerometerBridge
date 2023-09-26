#ifndef UDP_RECEIVER_H
#define UDP_RECEIVER_H

#include <QDebug>
#include "udp.h"
#include <thread>

class Udp_Receiver
{

public:
    Udp_Receiver();
    ~Udp_Receiver();
    udp_data udpOutData;
    void get_bias();
    void startReceive();
    void stopReceive();
    void threadReceive();
    void loopReceive();
    bool receive_finish = false;

private:
    bool isRunning = false;
    UDPReceiver udpreceiver;

};


#endif // UDP_RECEIVER_H
