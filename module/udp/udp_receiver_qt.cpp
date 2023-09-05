#include "udp_receiver_qt.h"

Udp_Receiver_Qt::Udp_Receiver_Qt(QObject *parent)
    :QObject(parent)
    ,isRunning(false)
{
    udpreceiver = new UDPReceiver;
}

Udp_Receiver_Qt::~Udp_Receiver_Qt()
{
    delete udpreceiver;
}

void Udp_Receiver_Qt::startReceive_()
{
    isRunning = true;
    while(isRunning)
    {
        udpreceiver->startReceive_new(udpOutData);
        emit dataReceived();
        // 让线程处理其他未处理事件
        QApplication::processEvents();
    }
}

void Udp_Receiver_Qt::stopReceive()
{
    isRunning = false;
}

void Udp_Receiver_Qt::emitUdpStatus()
{
    emit udpStatus(udp_status);
}

void Udp_Receiver_Qt::get_bias()
{
    udpreceiver->getBias();
}
