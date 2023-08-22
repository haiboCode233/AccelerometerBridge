#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QTimer>
#include "module/udp/udp.h"
#include "module/wave_view/mwaveview.h"
#include "module/udp/udp_receiver_qt.h"
#include "module/file_writer/file_writer_qt.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void readJson();
    void onHourlyTimeout();
    void onDataReceived(void* data);

    // 量程旋钮
    void on_dial_X_sliderMoved(int position);
    void on_dial_Y_sliderMoved(int position);

    // 通道开关
    void on_checkBox_0_stateChanged(int arg1);
    void on_checkBox_1_stateChanged(int arg1);
    void on_checkBox_2_stateChanged(int arg1);
    void on_checkBox_3_stateChanged(int arg1);

    // 接收启停开关
    void on_pushButton_pause_clicked();
    void on_pushButton_start_clicked();

    // 输出频率下拉框
    void on_comboBox_currentIndexChanged(int index);

    // 输出文件名修改框
    void on_lineEdit_textChanged(const QString &arg1);

    // 清除偏置（清零）按钮
    void on_pushButton_clicked();

    // 波形显示开关
    void on_pushButton_2_clicked();

private:
    // stateflag
    bool state;

    // ui
    Ui::MainWindow *ui;

    //object
    QTimer *timer;
    MWaveView *wave;
    Udp_Receiver_Qt *receiver;
    file_writer_qt *writer;

    // parameter
    int outputFrequency;
    QString outputFilename;

    // container
    QList<QPointF> wave_data[4];

    // thread
    QThread receiverThread;
    QThread writerThread;

};
#endif // MAINWINDOW_H
