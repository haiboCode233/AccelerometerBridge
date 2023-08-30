#include "netcheck.h"

NetCheck::NetCheck(QObject *parent)
    : QObject{parent}
{

}

void NetCheck::PeriodNetCheck()
{
    if(is_recv)  //按下了开始接收的按钮
    {
        qDebug()<<"连接断开！";
    }
    else
    {
        qDebug()<<"请开始接收！";
    }
}

void NetCheck::RecvBtnStatus(bool status)
{
    is_recv = status;
}
