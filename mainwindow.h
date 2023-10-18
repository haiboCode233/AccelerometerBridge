#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include "netwindow.h"
#include "udp.h"  // 不能删，删了报错winsock.h has been included
#include "module/net_check/netcheck.h"
#include "module/wave_view/mwaveview.h"
#include "module/api/api.h"
#include "module/api/api_thread.h"

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
    void call_onDataReceived();
    void onDataReceived_callback();
    void onDataReceived();
    // 关闭事件
    void closeEvent( QCloseEvent * event );

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
    void data_received_clr_timer();
    //  api
    void api_start_receive();
    void api_stop_receive();

    void on_time_minus_btn_clicked();

    void on_time_plus_btn_clicked();

    void on_voltage_minus_btn_clicked();

    void on_voltage_plus_btn_clicked();

private:
    // stateflag
    bool state;
    /*
     * t       num    index
     * 4000ms  8000   7
     * 2000ms  4000   6
     * 1000ms  2000   5
     * 500ms   1000   4
     * 200ms   400    3
     * 100ms   200    2
     * 50ms    100    1
     * 10ms    20     0
     *
     */
    int axis_x_index = 5;
    int axis_x_range[8] = {20,100,200,400,1000,2000,4000,8000};
    /*
     *  voltage  index
     *  1000mV   5
     *  500mV    4
     *  200mV    3
     *  100mV    2
     *  50mV     1
     *  10mV     0
     *
     */
    int axis_y_index = 2;
    double axis_y_range[6] = {0.01,0.05,0.1,0.2,0.5,1};

    // ui
    Ui::MainWindow *ui;

    //object
    QTimer *timer_1s;
    QTimer *timer_udp;
    MWaveView *wave;

    //class
    acqlib_api *qt_api;
    NetCheck *net_checker;

    // parameter
    int outputFrequency;
    QString outputFilename;

    //receiver
    Udp_Receiver *receiver;
    file_writer *writer;

    // container
    QList<QPointF> wave_data[4];
    QList<QPointF> wave_data_rolling[4];

    // QThread
    QThread receiverThread;
    QThread writerThread;


signals:
    void is_recvbtn_clicked(bool status);
    void start_recv();
    void stop_recv();
    void data_received();
    void start_process_data();

};
#endif // MAINWINDOW_H
