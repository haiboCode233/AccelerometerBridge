#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), net_state(false)
{
    readJson();

    // 接收对象
    receiver = new Udp_Receiver_Qt();
    receiver->moveToThread(&receiverThread);
    connect(receiver, &Udp_Receiver_Qt::dataReceived, this, &MainWindow::onDataReceived, Qt::QueuedConnection);
    receiverThread.start();

    // ui对象
    ui = new Ui::MainWindow();
    ui->setupUi(this);

    this->outputFrequency = 20;  // 2000hz  -->  20hz
    ui->lineEdit->setPlaceholderText("Default");

    //  图像对象
    wave = new MWaveView(ui->widget);
    wave->setRangeX(2000);  //  限制绘图总数据数量，不限制很容易卡顿
    wave->resize(ui->widget->size());

    //  写文件对象
    writer = new file_writer_qt();
    writer->moveToThread(&writerThread);
    writer->outputfilename = "data";

    //  网络检查
    net_checker = new NetCheck();
    timer_udp = new QTimer();
    timer_udp->start(1000);
    connect(timer_udp, &QTimer::timeout, net_checker, &NetCheck::PeriodNetCheck);  // 定时器启动并且达到定时时间，说明未接收到消息

    //  定时器 每秒检查 每一小时新建一个新文件
    timer_1s = new QTimer();
    timer_1s->start(1000);
    connect(timer_1s, &QTimer::timeout, this, &MainWindow::StartUdpTimer);
    connect(timer_1s, &QTimer::timeout, this, &MainWindow::onHourlyTimeout);
    connect(receiver, &Udp_Receiver_Qt::dataReceived, writer, &file_writer_qt::startwriteFile, Qt::QueuedConnection);
    writerThread.start();

    //  传递“接收按钮”的按下状态到网络检查
    connect(this, &MainWindow::is_recvbtn_clicked, net_checker, &NetCheck::RecvBtnStatus);

    if (this->state == 1)
    {
//        on_pushButton_clicked();
//        on_pushButton_start_pause_clicked();
//        on_pushButton_2_clicked();
        ;
    }
}

MainWindow::~MainWindow()
{
    // 停止线程并等待其退出
    receiverThread.quit();
    receiverThread.wait();
    writerThread.quit();
    writerThread.wait();



    delete ui;
    delete net_win;

    delete wave;
    delete writer;
    delete receiver;
    delete net_checker;

    delete timer_1s;
    delete timer_udp;


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
    if (currentDateTime.time().minute() == 0 && currentDateTime.time().second() == 0)   //
    {
        writer->outputfile.close(); // 先关闭上一个
        writer->outputfilename = currentDateTime.date().toString(Qt::ISODate) + "-" + currentDateTime.time().toString("hh");    // 获取当前时间作为文件名
        writer->outputfile.open(writer->outputfilename.toStdString()+".csv", std::ios_base::app);   // 新建一个文件
    }
}

// 回调函数
// 接收到数据后画图
void MainWindow::onDataReceived(void* data)
{
    float (*floatMatrix)[20] = static_cast<float(*)[20]>(data);
    MainWindow::net_state = true;
    static int param[4];
    static int count[4] = {0,0,0,0};  //  接收的数据计数器
    QPointF point;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            count[i] ++;
            if (count[i] % (2000/this->outputFrequency)/2 == 0)  // count[i]不断累加，Freq为50时候，(2000/50)/2 = 20, 也就是每20个数据采样一次
            {
                param[i]++;
                point.setY(floatMatrix[i][j]);
                point.setX(param[i]);
                this->wave_data[i].append(point);
                count[i] = 0;
            }
            while (this->wave_data[i].size() > 5000/*this->outputFrequency*500*/)
            {
                this->wave_data[i].removeFirst();
            }
        }
        wave->addSeriesData((WAVE_CH)i,this->wave_data[i]);
        timer_udp->stop();
    }
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


        QDateTime currentDateTime = QDateTime::currentDateTime();
        writer->outputfilename = currentDateTime.date().toString(Qt::ISODate) + "-" + currentDateTime.time().toString("hh");


        writer->outputfile.open(writer->outputfilename.toStdString()+".csv", std::ios_base::app);
        QMetaObject::invokeMethod(receiver, &Udp_Receiver_Qt::startReceive_);
        emit is_recvbtn_clicked(true);
    }
    else
    {
        ui->pushButton_start_pause->setText("开始接收");
        QMetaObject::invokeMethod(receiver, &Udp_Receiver_Qt::stopReceive);
        writer->outputfile.close();
        emit is_recvbtn_clicked(false);
    }
}

void MainWindow::on_pushButton_net_clicked()
{
    if(ui->pushButton_net->isChecked())
    {
        ui->pushButton_net->setDown(false);
        net_win = new netwindow();
        net_win->show();
    }
    else
    {
        ;
    }

}

// 设置输出频率
void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
        this->outputFrequency = 1;
        writer->outputfrequency = 1;
        break;
    case 1:
        this->outputFrequency = 20;
        writer->outputfrequency = 20;
        break;
    case 2:
        this->outputFrequency = 50;
        writer->outputfrequency = 50;
        break;
    case 3:
        this->outputFrequency = 100;
        writer->outputfrequency = 100;
        break;
    case 4:
        this->outputFrequency = 200;
        writer->outputfrequency = 200;
        break;
    }
}

// 设置输出文件名
void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    this->outputFilename = arg1;
    writer->outputfilename = arg1;

}

// 消除偏置
void MainWindow::on_pushButton_clicked()
{
    receiver->get_bias();

}

// 波形显示开关
void MainWindow::on_pushButton_2_clicked()
{
    if (ui->pushButton_2->text() == "打开显示")
    {
        ui->pushButton_2->setText("关闭显示");
        wave->startGraph();
    }
    else
    {
        ui->pushButton_2->setText("打开显示");
        wave->pauseGraph();
    }

}
