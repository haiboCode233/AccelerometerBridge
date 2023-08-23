#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    readJson();

    // ui对象
    ui = new Ui::MainWindow();
    ui->setupUi(this);

    this->outputFrequency = 20;  // 2000hz  -->  20hz
    ui->lineEdit->setPlaceholderText("Default");

    // 图像对象
    wave = new MWaveView(ui->widget);
    wave->resize(ui->widget->size());

    // 写文件对象
    writer = new file_writer_qt();
    writer->moveToThread(&writerThread);

    writer->outputfilename = "data";

    // 接收对象
    receiver = new Udp_Receiver_Qt();
    receiver->moveToThread(&receiverThread);

    // 定时器 每秒检查 每一小时新建一个新文件
    timer = new QTimer();
    timer->start(1000);
    connect(timer, &QTimer::timeout, this, &MainWindow::onHourlyTimeout);

    // 设置定时器间隔为1秒


    connect(receiver, &Udp_Receiver_Qt::dataReceived, this, &MainWindow::onDataReceived, Qt::QueuedConnection);
    connect(receiver, &Udp_Receiver_Qt::dataReceived, writer, &file_writer_qt::startwriteFile, Qt::QueuedConnection);


    receiverThread.start();
    writerThread.start();

    if (this->state == 1)
    {
        on_pushButton_clicked();
        on_pushButton_start_clicked();
        on_pushButton_2_clicked();

    }
}

MainWindow::~MainWindow()
{
    // 停止线程并等待其退出
    receiverThread.quit();
    writerThread.quit();
    writerThread.wait();

    delete ui;
    delete wave;
    delete writer;
    delete receiver;

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

void MainWindow::onHourlyTimeout()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    if (currentDateTime.time().minute() == 0 && currentDateTime.time().second() == 0)   //
    {
//        qDebug() << 1;
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

    static int param[4];
    static int count[4] = {0,0,0,0};
    QPointF point;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            count[i] ++;
            if (count[i] % (2000/this->outputFrequency)/2 == 0)
            {
                param[i]++;
                point.setY(floatMatrix[i][j]);
                point.setX(param[i]);
                this->wave_data[i].append(point);
                count[i] = 0;
            }
            while (this->wave_data[i].size() > this->outputFrequency*1000)
            {
                this->wave_data[i].removeFirst();
            }
        }
        wave->addSeriesData((WAVE_CH)i,this->wave_data[i]);
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

// 接收关
void MainWindow::on_pushButton_pause_clicked()
{
    QMetaObject::invokeMethod(receiver, &Udp_Receiver_Qt::stopReceive);
    wave->pauseGraph();
    writer->outputfile.close();
}

// 接收开
void MainWindow::on_pushButton_start_clicked()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    writer->outputfilename = currentDateTime.date().toString(Qt::ISODate) + "-" + currentDateTime.time().toString("hh");


    writer->outputfile.open(writer->outputfilename.toStdString()+".csv", std::ios_base::app);
    QMetaObject::invokeMethod(receiver, &Udp_Receiver_Qt::startReceive_);
    wave->startGraph();
}

// 设置输出频率
void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
        this->outputFrequency = 20;
        writer->outputfrequency = 20;
        break;
    case 1:
        this->outputFrequency = 50;
        writer->outputfrequency = 50;
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

