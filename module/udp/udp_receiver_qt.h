#ifndef UDP_RECEIVER_QT_H
#define UDP_RECEIVER_QT_H

#include <QObject>
#include <QThread>
#include <QPointF>
#include <QQueue>
#include <QDebug>
#include <QApplication>
#include "udp.h"

class Udp_Receiver_Qt : public QObject
{
    Q_OBJECT

public:
    Udp_Receiver_Qt(QObject *parent = nullptr);
    ~Udp_Receiver_Qt();
    std::vector<std::queue<double>> OutputData;
    udp_data udpOutData;
    float outputdata[4][20];
    QQueue<float> dataQQueuefloat;

    QList<QPointF> data[4];
    bool isRunning;

    void get_bias();

signals:
    void dataReceived();
    void udpStatus(bool status);

public slots:

    void startReceive_();
    void stopReceive();
    void emitUdpStatus();

private:
    UDPReceiver *udpreceiver;
    bool udp_status = true;



};


#endif // UDP_RECEIVER_QT_H
