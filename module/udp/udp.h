#ifndef UDP_H
#define UDP_H

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <asio.hpp>
#include <iomanip>
#include "filter.h"
#include <exception>
#include "udp_data.h"

class UDPReceiver {
public:
    // 构造函数
    UDPReceiver();
    // 析构函数
    ~UDPReceiver();

    // 单次接收函数
    void receive_once(int data_number);
    // 阻塞接收函数
    void receive_loop();
    // 单次接收函数（新）
    //void startReceive_new(std::vector<std::queue<float>>& OutputData);
    // 获取偏移
    int getBias();


    asio::io_context io_context_;                       // 异步控制器
    asio::ip::udp::socket socket_;                      // 套接字
    std::array<char, 5000> buffer_;                     // 接收buff
    char buffer_char[5000];

    std::vector<float> filter_;                        // 滤波器系数向量

    int receive_loop_flag_;                             // 循环标志
    int receive_count_;                                 // 已循环次数
    int data_per_bag_;                                  // 每包里数据组数

    std::vector<float> bias_;                          // 用于保存偏置
    std::queue<std::string> time_stamp_;                // 时间戳队列（1）
    std::vector<std::queue<float>> raw_data_;          // 原数据队列（4）
    std::vector<std::queue<float>> processed_data_;    // 处理后数据队列（4）

    std::ofstream output_file_;                          // 输出文件对象

    // 输出频率
    unsigned int outputfreq_;
    // 接收次数
    int receive_max;
    // 输出文件名
    std::string file_name_;

private:
    // 启动接收函数
    void startReceive();
    // 回调函数
    void handleReceive(const std::error_code& error, std::size_t bytes_transferred);


    int calculateAverage();
    // 解包数据到向量
    void handleReceivedData(const std::string input);
    // 卷积滤波
    void processRawData();
    // 队列内容显示函数
    void QueueDisplay(const std::vector<std::queue<float>>& queue);
    // 卷积
    std::queue<float> convolution(std::vector<float>& input, std::vector<float>& kernel);
    // 队列转向量
    std::vector<float> queueToVector(std::queue<float> queue);
    // 将队列中的数据写入CSV文件的末尾
    void writeToCSV();
    // 获取时间戳
    void getTimeStamp();


public:
    bool startReceive_new(udp_data &dat);
    int startReceive_new_async(float** output);
    int handleReceive_new_async(std::size_t bytes_transferred);
    int handleReceivedData_new(std::string input, float **output);

};

#endif  // UDP_H




