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
    float outputdata[4][20];
    QQueue<float> dataQQueuefloat;

    QList<QPointF> data[4];
    bool isRunning;

    void get_bias();

signals:
    void dataReceived(void* data);

public slots:

    void startReceive_();
    void stopReceive();

private:
    UDPReceiver *udpreceiver;
     // 用于控制循环是否运行的标志


};


#endif // UDP_RECEIVER_QT_H
