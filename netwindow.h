#ifndef NETWINDOW_H
#define NETWINDOW_H

#include <QWidget>
#include <QCloseEvent>
#include <qdebug.h>

namespace Ui {
class netwindow;
}

class netwindow : public QWidget
{
    Q_OBJECT

public:
    explicit netwindow(QWidget *parent = nullptr);

    ~netwindow();

protected:
    void closeEvent(QCloseEvent *e);

private:
    Ui::netwindow *ui;

signals:
    void sig_netbtn_work();
};

#endif // NETWINDOW_H
