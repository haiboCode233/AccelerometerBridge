#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    readJson();
    //api
    qt_api = new acqlib_api();
    qt_api->acqlib_init();
    auto callback = [this]() {
        onDataReceived();
    };
    std::thread thread_receiver(&Udp_Receiver::loopReceive, &qt_api->receiver, callback);
    thread_receiver.detach();

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

    connect(timer_udp, &QTimer::timeout, net_checker, &NetCheck::PeriodNetCheck);  // 定时器启动并且达到定时时间，说明未接收到消息
    connect(timer_1s, &QTimer::timeout, this, &MainWindow::StartUdpTimer);
    connect(timer_1s, &QTimer::timeout, this, &MainWindow::onHourlyTimeout);
    connect(net_win, &netwindow::sig_netbtn_work, this, &MainWindow::set_netbtn_work);  //  网络窗口关闭释放网络按钮
    connect(this, &MainWindow::is_recvbtn_clicked, net_checker, &NetCheck::RecvBtnStatus);  //  传递“接收按钮”的按下状态到网络检查
    connect(this, &MainWindow::start_recv, this, &MainWindow::api_start_receive);
    connect(this, &MainWindow::stop_recv, this, &MainWindow::api_stop_receive);
    connect(this,&MainWindow::data_received,this, &MainWindow::data_received_clr_timer);
}

MainWindow::~MainWindow()
{
    qt_api->acqlib_deinit();
    delete qt_api;
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

// 每小时创建一个文件存储数据
void MainWindow::onHourlyTimeout()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    if (currentDateTime.time().minute() == 0 && currentDateTime.time().second() == 0)
    {
        qt_api->writer.outputfile.close(); // 先关闭上一个
        qt_api->writer.getNowTime();
        qt_api->writer.outputfile.open(qt_api->writer.outPutFileName, std::ios_base::app);
    }
}

// 接收到数据后画图
void MainWindow::onDataReceived()
{
    qt_api->receiver.receive_finish = false;
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
                if(param[i] >= 2000)
                {
                    param[i] = 0;
                }
                point.setY(qt_api->acqlib_get_data(i,j));
                point.setX(param[i]);
                this->wave_data[i].append(point);
                count[i] = 0;
            }
            if(this->wave_data[i].size() >= 2000)  // 限定固定缓冲区范围，防止缓冲区过大（原本是与刷新率成正比）
            {
                this->wave_data[i].removeFirst();
            }

        }
        wave->addSeriesData((WAVE_CH)i,this->wave_data[i]);
    }
    emit data_received();
}

// 处理旋钮函数
int limit(int value, int last_value, int size)
{
    int dir =0;
    int tmp = value - last_value;
    if ((tmp>0&& tmp< (size/2)) ||(tmp< -(size/2)))
    {
        dir = 1;
    }
    else if ((tmp<0 && tmp>-(size/2)) || (tmp > (size/2)))
    {
        dir = -1;
    }
    return dir;
}

void MainWindow::on_dial_X_sliderMoved(int position)
{
    static int last_value = 5;
    int now_ret = 0;

    now_ret = limit(position,last_value,10);
    last_value = position;

    if(now_ret>0){
        wave->ZoomX();
    }
    else {
        wave->ZoomOutX();
    }
}

void MainWindow::on_Xslider_valueChanged(int value)
{
    static int value_last = 50;
    int value_reserve = value;
    if(value > value_last)
    {
        while(((value - value_last)/5))
        {
            value = value - 5;
            wave->ZoomOutX();
        }
    }
    else
    {
        while((value_last - value)/5)
        {
            value_last = value_last - 5;
            wave->ZoomX();
        }
    }
    value_last = value_reserve;
}

void MainWindow::on_YSlider_valueChanged(int value)
{
    static int value_last = 50;
    int value_reserve = value;
    if(value > value_last)
    {
        while(((value - value_last)/5))
        {
            value = value - 5;
            wave->ZoomOutY();
        }
    }
    else
    {
        while((value_last - value)/5)
        {
            value_last = value_last - 5;
            wave->ZoomY();
        }
    }
    value_last = value_reserve;
}

void MainWindow::on_dial_Y_sliderMoved(int position)
{
    static int last_value = 5;
    int now_ret = 0;

    now_ret = limit(position,last_value,10);
    last_value = position;

    if(now_ret>0){
        wave->ZoomY();
    }
    else {
        wave->ZoomOutY();
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
        qt_api->writer.getNowTime();
        qt_api->writer.outputfile.open(qt_api->writer.outPutFileName, std::ios_base::app);
        emit start_recv();
        emit is_recvbtn_clicked(true);
    }
    else
    {
        ui->pushButton_start_pause->setText("开始接收");
        qt_api->writer.outputfile.close();
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
        qt_api->acqlib_samp_freq(20);
        break;
    case 1:
        qt_api->acqlib_samp_freq(100);
        break;
    case 2:
        qt_api->acqlib_samp_freq(200);
        break;
    case 3:
        qt_api->acqlib_samp_freq(500);
        break;
    case 4:
        qt_api->acqlib_samp_freq(1000);
        break;
    case 5:
        qt_api->acqlib_samp_freq(2000);
        break;
    }
}

// 设置输出文件名
void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    this->outputFilename = arg1;
    qt_api->writer.outPutFileName = arg1.toStdString();
}

// 消除偏置
void MainWindow::on_pushButton_clicked()
{
    qt_api->receiver.get_bias();

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
    qt_api->acqlib_start_receive();
}

void MainWindow::api_stop_receive()
{
    qt_api->acqlib_stop_receive();
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
