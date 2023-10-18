#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    readJson();
    //api

    receiver = new Udp_Receiver;
    receiver->moveToThread(&receiverThread);
    writer = new file_writer;

    //  网络检查
    net_checker = new NetCheck();
    timer_udp = new QTimer();
    timer_udp->start(1000);

    // ui对象
    ui = new Ui::MainWindow();
    ui->setupUi(this);
    net_win = new netwindow();
    ui->lineEdit->setPlaceholderText("Default");

    //  图像对象
    wave = new MWaveView(ui->widget);
    wave->setRangeX(2000);  //  限制绘图总数据数量，不限制很容易卡顿
    wave->resize(ui->widget->size());

    //  定时器 每秒检查 每一小时新建一个新文件
    timer_1s = new QTimer();
    timer_1s->start(1000);

    connect(receiver, &Udp_Receiver::data_received, this, &MainWindow::onDataReceived, Qt::QueuedConnection);
    connect(timer_udp, &QTimer::timeout, net_checker, &NetCheck::PeriodNetCheck);  // 定时器启动并且达到定时时间，说明未接收到消息
    connect(timer_1s, &QTimer::timeout, this, &MainWindow::StartUdpTimer);
    connect(timer_1s, &QTimer::timeout, this, &MainWindow::onHourlyTimeout);
    connect(net_win, &netwindow::sig_netbtn_work, this, &MainWindow::set_netbtn_work);  //  网络窗口关闭释放网络按钮
    connect(this, &MainWindow::is_recvbtn_clicked, net_checker, &NetCheck::RecvBtnStatus);  //  传递“接收按钮”的按下状态到网络检查
    connect(this, &MainWindow::start_recv, this, &MainWindow::api_start_receive);
    connect(this, &MainWindow::stop_recv, this, &MainWindow::api_stop_receive);

    receiverThread.start();
}

MainWindow::~MainWindow()
{
    receiver->stopReceive();
    delete receiver;
    delete writer;
    delete net_checker;
    delete timer_udp;
    delete ui;
    delete net_win;
    delete wave;
    delete timer_1s;
}

void MainWindow::readJson()
{
    QFile file("./test.json");
    if(!file.open(QIODevice::ReadOnly| QIODevice::Text))
    {
        this->state = 0;
        return;
    }
    QByteArray array = file.readAll(); //将数据读到array中

    QJsonParseError json_error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(array,&json_error);

    if(json_error.error == QJsonParseError::NoError)
    {
        QJsonObject rootObj = jsonDoc.object();

        if(rootObj.contains("state") && rootObj.value("state") == 1)
            this->state = 1;
        else
            this->state = 0;
        return;
    }
    this->state = 0;
}

void MainWindow::StartUdpTimer(void)
{
    timer_udp->start(1000);
}

void MainWindow::call_onDataReceived()
{
    emit start_process_data();
}

// 接收到数据后画图
void MainWindow::onDataReceived_callback()
{
    qt_api->acqlib_write_file();
    static int param[4];
    static int count[4] = {0,0,0,0};  //  接收的数据计数器
    QPointF point;
    for (int i = 0; i < 4; i++)  // 四个通道
    {
        for (int j = 0; j < 20; j++)
        {
            count[i] ++;
            if ((count[i] % (2000/qt_api->writer.outputfrequency)) == 0)  // 采样率 假设outputFrequency为50，那count[i]每40个取一个，2000个里就是取50个
            {
                param[i]++;
                point.setY(qt_api->acqlib_get_data(i,j));
                point.setX(param[i]);
                this->wave_data[i].append(point);
                count[i] = 0;
            }
            if(this->wave_data[i].size() > 2000)  // 大于2000个点后需要开始滚动
            {
                this->wave_data[i].clear();
                param[i] = 0;

            }

        }
        wave->addSeriesData((WAVE_CH)i,this->wave_data[i]);
    }
    emit data_received();
}

void MainWindow::onDataReceived()
{
    writer->startwriteFile(receiver->udpOutData);
    static int param[4];
    static int count[4] = {0,0,0,0};  //  接收的数据计数器
    QPointF point;
    for (int i = 0; i < 4; i++)  // 四个通道
    {
        for (int j = 0; j < 20; j++)
        {
            count[i] ++;
            if ((count[i] % (2000/writer->outputfrequency)) == 0)  // 采样率 假设outputFrequency为50，那count[i]每40个取一个，2000个里就是取50个
            {
                param[i]++;
                point.setY(receiver->udpOutData.readFromArray()[i][j]);
                point.setX(param[i]);
                this->wave_data[i].append(point);
                count[i] = 0;
            }
            if(this->wave_data[i].size() > this->axis_x_range[axis_x_index])  // 大于2000个点后需要开始滚动
            {
                this->wave_data[i].clear();
                param[i] = 0;
            }

        }
        wave->addSeriesData((WAVE_CH)i,this->wave_data[i]);
    }
    timer_udp->stop();
    emit data_received();
}

void MainWindow::onHourlyTimeout()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    if (currentDateTime.time().minute() == 0 && currentDateTime.time().second() == 0)   //
    {
        writer->outputfile.close(); // 先关闭上一个
        writer->autoFileName();
        writer->outputfile.open(writer->outPutFileName, std::ios_base::app);
    }
}

// 处理通道开关
void MainWindow::on_checkBox_0_stateChanged(int arg1)
{
    if(arg1){
        wave->openChannel(WAVE_CH0);
    }
    else{
        wave->closeChannel(WAVE_CH0);
    }
}

void MainWindow::on_checkBox_1_stateChanged(int arg1)
{
    if(arg1){
        wave->openChannel(WAVE_CH1);
    }
    else{
        wave->closeChannel(WAVE_CH1);
    }
}

void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
    if(arg1){
        wave->openChannel(WAVE_CH2);
    }
    else{
        wave->closeChannel(WAVE_CH2);
    }
}

void MainWindow::on_checkBox_3_stateChanged(int arg1)
{
    if(arg1){
        wave->openChannel(WAVE_CH3);
    }
    else{
        wave->closeChannel(WAVE_CH3);
    }
}

// 接收开关
void MainWindow::on_pushButton_start_pause_clicked()
{
    if (ui->pushButton_start_pause->text() == "开始接收")
    {
        ui->pushButton_start_pause->setText("关闭接收");
        writer->autoFileName();
        writer->outputfile.open(writer->outPutFileName, std::ios_base::app);
        QMetaObject::invokeMethod(receiver, &Udp_Receiver::loopReceive);
        emit start_recv();
        emit is_recvbtn_clicked(true);
    }
    else
    {
        ui->pushButton_start_pause->setText("开始接收");
        writer->outputfile.close();
        emit stop_recv();
        emit is_recvbtn_clicked(false);
    }
}

void MainWindow::on_pushButton_net_clicked()
{
    ui->pushButton_net->setEnabled(false);
    net_win->show();
}


void MainWindow::set_netbtn_work()
{
    ui->pushButton_net->setEnabled(true);
}

// 设置输出频率
void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
        writer->outputfrequency = 20;
        break;
    case 1:
        writer->outputfrequency = 100;
        break;
    case 2:
        writer->outputfrequency = 200;
        break;
    case 3:
        writer->outputfrequency = 500;
        break;
    case 4:
        writer->outputfrequency = 1000;
        break;
    case 5:
        writer->outputfrequency = 2000;
        break;
    }
}

// 设置输出文件名
void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    this->outputFilename = arg1;
    writer->outPutFileName = arg1.toStdString();
}

// 消除偏置
void MainWindow::on_pushButton_clicked()
{
    receiver->get_bias();

}

// 波形显示开关
void MainWindow::on_pushButton_wave_clicked()
{
    if (ui->pushButton_wave->text() == "▶")
    {
        ui->pushButton_wave->setText("||");
        wave->startGraph();
    }
    else
    {
        ui->pushButton_wave->setText("▶");
        wave->pauseGraph();
    }
}

void MainWindow::data_received_clr_timer()
{
    timer_udp->stop();
}


void MainWindow::api_start_receive()
{
    receiver->startReceive();
}

void MainWindow::api_stop_receive()
{
    receiver->stopReceive();
}

void MainWindow::closeEvent( QCloseEvent * event )//关闭窗口弹窗
{
    switch( QMessageBox::information( this,
            tr("退出警告"),
            tr("您是否需要关闭采集软件？"),
            tr("关闭"), tr("取消"),0, 1 ) )
    {
        case 0:
            event->accept();
            break;
        case 1:
            ;
        default:
            event->ignore();
            break;
    }
}


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
void MainWindow::on_time_minus_btn_clicked()
{
    if(axis_x_index - 1 < 0)
    {
        ;
    }
    else
    {
        axis_x_index--;
    }
    switch(axis_x_index)
    {
    case 0:
        ui->label_time->setText("时间尺度：10ms");
        break;
    case 1:
        ui->label_time->setText("时间尺度：50ms");
        break;
    case 2:
        ui->label_time->setText("时间尺度：100ms");
        break;
    case 3:
        ui->label_time->setText("时间尺度：200ms");
        break;
    case 4:
        ui->label_time->setText("时间尺度：500ms");
        break;
    case 5:
        ui->label_time->setText("时间尺度：1000ms");
        break;
    case 6:
        ui->label_time->setText("时间尺度：2000ms");
        break;
    case 7:
        ui->label_time->setText("时间尺度：4000ms");
        break;
    default:
        ui->label_time->setText("error: no such index");
        break;
    }
    wave->setRangeX(0, this->axis_x_range[axis_x_index]);
}


void MainWindow::on_time_plus_btn_clicked()
{
    if(axis_x_index + 1 > 7)
    {
        ;
    }
    else
    {
        axis_x_index++;
    }
    switch(axis_x_index)
    {
    case 0:
        ui->label_time->setText("时间尺度：10ms");
        break;
    case 1:
        ui->label_time->setText("时间尺度：50ms");
        break;
    case 2:
        ui->label_time->setText("时间尺度：100ms");
        break;
    case 3:
        ui->label_time->setText("时间尺度：200ms");
        break;
    case 4:
        ui->label_time->setText("时间尺度：500ms");
        break;
    case 5:
        ui->label_time->setText("时间尺度：1000ms");
        break;
    case 6:
        ui->label_time->setText("时间尺度：2000ms");
        break;
    case 7:
        ui->label_time->setText("时间尺度：4000ms");
        break;
    default:
        ui->label_time->setText("error: no such index");
        break;
    }
    wave->setRangeX(0, this->axis_x_range[axis_x_index]);
}

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
void MainWindow::on_voltage_minus_btn_clicked()
{
    if(axis_y_index - 1 < 0)
    {
        ;
    }
    else
    {
        axis_y_index--;
    }
    switch(axis_y_index)
    {
    case 0:
        ui->label_voltage->setText("幅值尺度：10mV");
        break;
    case 1:
        ui->label_voltage->setText("幅值尺度：50mV");
        break;
    case 2:
        ui->label_voltage->setText("幅值尺度：100mV");
        break;
    case 3:
        ui->label_voltage->setText("幅值尺度：200mV");
        break;
    case 4:
        ui->label_voltage->setText("幅值尺度：500mV");
        break;
    case 5:
        ui->label_voltage->setText("幅值尺度：1000mV");
        break;
    default:
        ui->label_voltage->setText("error: no such index");
        break;
    }
    wave->setRangeY(-this->axis_y_range[axis_y_index], this->axis_y_range[axis_y_index]);
}


void MainWindow::on_voltage_plus_btn_clicked()
{
    if(axis_y_index + 1 > 5)
    {
        ;
    }
    else
    {
        axis_y_index++;
    }
    switch(axis_y_index)
    {
    case 0:
        ui->label_voltage->setText("幅值尺度：10mV");
        break;
    case 1:
        ui->label_voltage->setText("幅值尺度：50mV");
        break;
    case 2:
        ui->label_voltage->setText("幅值尺度：100mV");
        break;
    case 3:
        ui->label_voltage->setText("幅值尺度：200mV");
        break;
    case 4:
        ui->label_voltage->setText("幅值尺度：500mV");
        break;
    case 5:
        ui->label_voltage->setText("幅值尺度：1000mV");
        break;
    default:
        ui->label_voltage->setText("error: no such index");
        break;
    }
    wave->setRangeY(-this->axis_y_range[axis_y_index], this->axis_y_range[axis_y_index]);
}

