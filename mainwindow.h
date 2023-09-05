#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QTimer>
#include "netwindow.h"
#include "udp.h"  // 不能删，删了报错winsock.h has been included
#include "module/net_check/netcheck.h"
#include "module/wave_view/mwaveview.h"
#include "module/file_writer/file_writer_qt.h"
#include "module/udp/udp_receiver_qt.h"
#include "module/udp/udp_data.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    netwindow *net_win;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void readJson();
    void onHourlyTimeout();
    void StartUdpTimer(void);
    void onDataReceived();

    // 量程旋钮
    void on_dial_X_sliderMoved(int position);
    void on_dial_Y_sliderMoved(int position);

    // 通道开关
    void on_checkBox_0_stateChanged(int arg1);
    void on_checkBox_1_stateChanged(int arg1);
    void on_checkBox_2_stateChanged(int arg1);
    void on_checkBox_3_stateChanged(int arg1);

    // 接收启停开关
    void on_pushButton_start_pause_clicked();

    // 网络页面
    void on_pushButton_net_clicked();

    // 输出频率下拉框
    void on_comboBox_currentIndexChanged(int index);

    // 输出文件名修改框
    void on_lineEdit_textChanged(const QString &arg1);

    // 清除偏置（清零）按钮
    void on_pushButton_clicked();

    // 波形显示开关
    void on_pushButton_wave_clicked();


    // 设置网络按钮可用
    void set_netbtn_work();

    void on_Xslider_valueChanged(int value);

    void on_YSlider_valueChanged(int value);

private:
    // stateflag
    bool state;
    bool net_state;

    // ui
    Ui::MainWindow *ui;

    //object
    QTimer *timer_1s;
    QTimer *timer_udp;
    MWaveView *wave;
    Udp_Receiver_Qt *receiver;
    file_writer_qt *writer;

    //class
    NetCheck *net_checker;

    // parameter
    int outputFrequency;
    QString outputFilename;

    // container
    QList<QPointF> wave_data[4];

    // thread
    QThread receiverThread;
    QThread writerThread;


signals:
    void is_recvbtn_clicked(bool status);

};
#endif // MAINWINDOW_H
