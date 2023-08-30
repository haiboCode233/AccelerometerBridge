#ifndef NETCHECK_H
#define NETCHECK_H

#include <QObject>
#include <QTimer>
#include <QDebug>

class NetCheck : public QObject
{
    Q_OBJECT
public:
    explicit NetCheck(QObject *parent = nullptr);

    void PeriodNetCheck();

    void RecvBtnStatus(bool status);
private:
    bool is_recv = false;




signals:

};

#endif // NETCHECK_H
