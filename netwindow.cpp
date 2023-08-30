#include "netwindow.h"
#include "ui_netwindow.h"

netwindow::netwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::netwindow)
{
    ui->setupUi(this);
}

void netwindow::closeEvent(QCloseEvent *e)
{
    emit sig_netbtn_work();
}

netwindow::~netwindow()
{
    delete ui;
}
