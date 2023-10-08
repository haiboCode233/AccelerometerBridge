#include "module/api/api.h"

acqlib_api::acqlib_api()
{
    ;
}

acqlib_api::~acqlib_api()
{
    ;
}

int acqlib_api::acqlib_init()
{
    acqlib_samp_freq(20);
    return 0;
}


int acqlib_api::acqlib_deinit()
{
    return 0;
}

float acqlib_api::acqlib_get_data(int row, int col)
{
    return receiver.udpOutData.readFromArray()[row][col];
}

int acqlib_api::acqlib_write_file()
{
    writer.startwriteFile(receiver.udpOutData);
    return 0;
}


/*
 * 功能：开启接收线程，每次运行完毕后执行回调函数callback_func来处理数据
 *      数据获取使用函数    “acqlib_get_data(i,j)”
 *
 * 案例：①创建api实例      “acqlib_api *qt_api;  qt_api = new acqlib_api();”
 *      ②★创建目标回调函数  “auto callback = [ClassPointer_of_onDataReceived]() {  onDataReceived(); };”
 *      ③调用本函数       “qt_api->acqlib_active_receiver_thread(qt_api,callback);”
 */
int acqlib_api::acqlib_active_receiver_thread(acqlib_api * api, CallbackFunction callback_func)
{
    std::thread thread_receiver(&Udp_Receiver::loopReceive, &api->receiver, callback_func);
    thread_receiver.detach();
    return 0;
}

int acqlib_api::acqlib_file_timer()
{
    static int now_sec = 0;
    static int last_sec = 0;
    while(true)
    {
        if(this->file_timer_start)
        {
            time_t rawtime;
            struct tm * timeinfo;
            char buffer[80];

            time(&rawtime);
            timeinfo = localtime(&rawtime);
            last_sec = now_sec;
            now_sec = timeinfo->tm_sec;
            if(last_sec != now_sec)  //每秒执行一次
            {
                if(timeinfo->tm_sec == 0 && timeinfo->tm_min == 0)  // 每小时创建一个文件
                {
                    writer.outputfile.close(); // 先关闭上一个
                    writer.getNowTime();
                    writer.outputfile.open(writer.outPutFileName, std::ios_base::app);
                }
                //  strftime(buffer, 80, "%Y-%m-%d-%H-%M-%S", timeinfo);
                //  std::cout << "Now time: " << buffer << std::endl;
            }
        }
        else
        {
            ;
        }
    }
}

int acqlib_api::acqlib_start_file_timer()
{
    this->file_timer_start = true;
    return 0;
}

int acqlib_api::acqlib_stop_file_timer()
{
    this->file_timer_start = false;
    return 0;
}

/*
 * 功能：开启创建文件线程，每一小时创建一个文件
 *
 * 案例：①创建api实例     “acqlib_api *qt_api;  qt_api = new acqlib_api();”
 *      ②调用本函数      “qt_api->acqlib_active_datastorage_thread(qt_api);”
 */
int acqlib_api::acqlib_active_datastorage_thread(acqlib_api * api)
{
    std::thread thread_file_timer(&acqlib_api::acqlib_file_timer, api);
    thread_file_timer.detach();
    return 0;
}

int acqlib_api::acqlib_start_receive()
{
    receiver.startReceive();
    return 0;
}

int acqlib_api::acqlib_stop_receive()
{
    receiver.stopReceive();
    return 0;
}

int acqlib_api::acqlib_samp_freq(int freq)
{
    if(freq > MAX_SAMP_FREQ)
    {
        freq = MAX_SAMP_FREQ;
    }
    if(freq < MIN_SAMP_FREQ)
    {
        freq = MIN_SAMP_FREQ;
    }
    writer.outputfrequency = freq;
    return 0;
}

