#ifndef UDP_RECEIVER_H
#define UDP_RECEIVER_H

#include <QDebug>
#include <QObject>
#include "udp.h"
#include <thread>

class Udp_Receiver :public QObject
{
    Q_OBJECT

public:
    Udp_Receiver(QObject *parent = nullptr);
    ~Udp_Receiver();
    udp_data udpOutData;
    void get_bias();
    void startReceive();
    void stopReceive();
    void threadReceive();
    void loopReceive_callback(std::function<void()> callback); //thread_api version
    void loopReceive(); //thread_api version

private:
    bool isRunning = false;
    UDPReceiver udpreceiver;
signals:
    void data_received();

};


#endif // UDP_RECEIVER_H
