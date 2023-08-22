#include "udp.h"
#include <future>

#define ChannelSize 4

// 构造函数
UDPReceiver::UDPReceiver():
    socket_(io_context_, asio::ip::udp::endpoint(asio::ip::udp::v4(), 1122)),
    bias_(ChannelSize),
    raw_data_(ChannelSize),
    processed_data_(ChannelSize)
{
    // 初始化 filter_
    Filter fir;
    fir.order = 200;
    fir.freq_s = 2000;
    fir.freq_c = 100;
    filter_ = fir.generate_Filter();

    // 初始化 bias_
    for (int i = 0; i < ChannelSize; i++)
    {
        bias_[i] = 0.0;
    }
//    getBias();

    // 初始化 raw_data_ (卷积需要)
    for (int i = 0; i < ChannelSize; i++)
    {
        for (int j = 0; j < filter_.size()-1; j++)
        {
            raw_data_[i].push(0);
        }
    }

}

// 析构函数
UDPReceiver::~UDPReceiver()
{
    output_file_.close();
}

// 启动接收函数
void UDPReceiver::startReceive()
{
    socket_.async_receive(asio::buffer(buffer_),
        [this](const std::error_code& error, std::size_t bytes_transferred)
        {
            handleReceive(error, bytes_transferred);
        });
}

// 回调函数(原)
void UDPReceiver::handleReceive(const std::error_code& error, std::size_t bytes_transferred)
{
    if (!error)
    {   
        // 获取该循环的时间戳，并转化为string    
        getTimeStamp();

        std::string received_data(buffer_.data(), bytes_transferred);

        auto time1 = std::chrono::high_resolution_clock::now();     // test

        // 处理接收到的数据
        handleReceivedData(received_data);


        // 滚动处理队列中的数据
        processRawData();

        auto time2 = std::chrono::high_resolution_clock::now();     // test
        
        // 显示处理后的队列数据
        // QueueDisplay(processed_data_);
        // std::async(std::launch::async, calculateSum, 10, 20);
        // 写文件 同时清理旧数据
        std::async(std::launch::async, [this]()
        {
            writeToCSV();
        });
        // writeToCSV();

        auto time3 = std::chrono::high_resolution_clock::now();     // test

        auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(time2 - time1);
        auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(time3 - time2);
        auto duration3 = std::chrono::duration_cast<std::chrono::microseconds>(time3 - time1);

        // if (duration3.count() > 500*data_per_bag_-1000)
        // {
        //     std::cout << data_per_bag_ << std::endl;
        //     std::cout << "out of time" << std::endl;
        //     std::cout << "processing time: " << duration1.count() << " us" << std::endl;
        //     std::cout << "write file time: " << duration2.count() << " us" << std::endl;
        //     std::cout << "whole time: " << duration3.count() << " us" << std::endl;
        //     std::cout << std::endl;
        // }
                
        // std::cout << "processing time: " << duration2.count() << " us" << std::endl;
        // 继续接收数据
        startReceive();
    }
    else
    {
        std::cout << "Receive error: " << error.message() << std::endl;
    }
    
}

// 获取偏移
int UDPReceiver::getBias()
{
    int times = 100;
    for (int k = 0; k < times; k++)
    {
        // 接收数据并返回字节数
        std::size_t bytes_transferred = socket_.receive(asio::buffer(buffer_));
        std::string received_data(buffer_.data(), bytes_transferred);
        std::istringstream iss(received_data);  // 创建字符串流
        std::string line;
        data_per_bag_ = received_data.size()/50;
        for (int i = 0; i < data_per_bag_; i++)  // 按行读取字符串
        {
            std::getline(iss, line);
            size_t startPos = line.find(":");   // 去除每行开头的"data: "部分
            line = line.substr(startPos + 2);
            std::istringstream lineStream(line);  // 创建每行的字符串流
            std::string value;
            for (int j = 0; j < ChannelSize; j++)  // 按列处理数据
            {
                if (std::getline(lineStream, value, ','))  // 按逗号分割字符串
                {
                    float number = std::strtod(value.c_str(), nullptr);  // 将字符串转换为 float

                    bias_[j] += number;
                }
            }
        }
    }
    for (int i = 0; i < 4; i++)
    {
        bias_[i] = bias_[i] / (times*data_per_bag_);
        // std::cout << "bias_" << i << ": " << std::fixed << bias_[i] << "\n";
    }
    return 0;
}

int UDPReceiver::calculateAverage()
{
    for (int i = 0; i < ChannelSize; i++)
    {
        float sum = 0.0;
        float average = 0.0;
        int QueueSize = raw_data_[i].size();

        std::queue<float> tempQueue = raw_data_[i];

        while (!tempQueue.empty())
        {
            sum += tempQueue.front();
            tempQueue.pop();
        }

        bias_[i] = sum / QueueSize;
    }
    return 0;
}


// 解包数据到向量
void UDPReceiver::handleReceivedData(const std::string input)
{
    std::istringstream iss(input);  // 创建字符串流
    std::string line;
    data_per_bag_ = input.size()/50;

    // std::cout << input.size() << std::endl;
    for (int i = 0; i < data_per_bag_; i++)  // 按行读取字符串
    {
        std::getline(iss, line);
        size_t startPos = line.find(":");   // 去除每行开头的"data: "部分

        if (startPos != std::string::npos)  // 有效数据
        {
            line = line.substr(startPos + 2);
            std::istringstream lineStream(line);  // 创建每行的字符串流
            std::string value;
            for (int j = 0; j < ChannelSize; j++)  // 按列处理数据
            {
                if (std::getline(lineStream, value, ','))  // 按逗号分割字符串
                {
                    float number = std::strtof(value.c_str(), nullptr);  // 将字符串转换为 float
                    // std::cout << std::fixed << number << " ";    // *test
                    raw_data_[j].push(number - bias_[j]);
                }
            }
            // std::cout << std::endl;  // *test
        } 
        else    // 无效数据
        {
            for (int j = 0; j < ChannelSize; j++)  // 按列处理数据
            {
                float number = raw_data_[j].back();
                raw_data_[j].push(number - bias_[j]);
            }
        }
    }
}

// 卷积滤波
void UDPReceiver::processRawData()
{
    for (int i = 0; i < ChannelSize; i++)   // 各通道分别进行卷积
    {
        // std::cout << "Channel_" << i << ": ";   // *test
        std::vector<float> input = queueToVector(raw_data_[i]);
        std::queue<float> queue_temp = convolution(input, filter_);

        for (int j = 0; j < data_per_bag_; j++)    // 压入data_per_bag_组数据，丢掉data_per_bag_组旧数据
        {
            processed_data_[i].push(queue_temp.front());
            queue_temp.pop();
            raw_data_[i].pop();
        }
    }
    // std::cout << std::endl; // *test
}

// 队列内容显示函数
void UDPReceiver::QueueDisplay(const std::vector<std::queue<float>>& queue)
{
    for (int i = 0; i < queue.size(); i++)
    {
        std::queue<float> queueCopy = queue[i];
        std::cout << "Channel " << i << ": ";
        while (!queueCopy.empty())
        {
            std::cout << std::fixed << queueCopy.front() << " ";
            queueCopy.pop();
        }
        std::cout << std::endl;
    }
}
// 卷积
std::queue<float> UDPReceiver::convolution(std::vector<float>& input, std::vector<float>& kernel)
{
    // int inputSize = input.size();
    // int kernelSize = kernel.size();

    // std::cout << "input.size():" << input.size() << std::endl;
    // std::cout << "kernel.size():" << kernel.size() << std::endl;
    std::queue<float> processed_data_temp;
    for (int i = 0; i < data_per_bag_; i++)    // 每次处理出data_per_bag_个数据
    {
        float result = 0.0;
        for (int j = 0; j < kernel.size(); j++)     //进行卷积核大小次数的乘加操作
        {
            result += kernel[j] * input[i+j];
        }
        // std::cout << std::setw(15) << result;     // *test
        processed_data_temp.push(result);
        // std::cout << std::setw(15) << std::fixed << processed_data_temp.back();   // *test
    } 
    // std::cout << std::endl;    // *test
    return processed_data_temp;       
}
// 队列转向量
std::vector<float> UDPReceiver::queueToVector(std::queue<float> queue)
{
    std::vector<float> vector(queue.size());
    size_t i = 0;
    while (!queue.empty()) {
        vector[i] = queue.front();
        queue.pop();
        i++;
    }
    return vector;
}
// 将队列中的数据写入CSV文件的末尾
void UDPReceiver::writeToCSV()
{
    // 检查文件是否成功打开
    if (!output_file_.is_open()) {
        std::cout << "open error" << std::endl;
        return;
    }

    // 从向量中依次取出队列数据并写入文件
    int processedDataSize = processed_data_[0].size();
    if (processedDataSize >= 1000)
    {
        while (processed_data_[0].size())
        {
            std::string time = time_stamp_.front();
            time_stamp_.pop();
            if (processed_data_[0].size() % (2000/outputfreq_)  == 0 && (receive_count_ < receive_max | receive_loop_flag_))
            {
                // std::cout << receive_count_ << std::endl;
                ++receive_count_;
                output_file_ << time << ",";
                // std::cout << "2" << std::endl;
                for (int i = 0; i < ChannelSize; i++)   // 写入一行
                {
                    float data = processed_data_[i].front();
                    processed_data_[i].pop();
                    output_file_ << std::fixed << data << ",";
                }
                output_file_ << "\n";
                // output_file_ << std::endl;
            }
            else
            {
                for (int i = 0; i < ChannelSize; i++)
                {
                    processed_data_[i].pop();
                }
            }
        }
        output_file_.flush();
    }
    if (receive_count_ == receive_max)
    {
        receive_count_ = 0;
        io_context_.stop();
        
        std::cout << "stop receive" << std::endl;
    }

        
}
// 获取时间戳
void UDPReceiver::getTimeStamp() 
{
    // 获取当前时间
    auto timePoint = std::chrono::high_resolution_clock::now();

    // 将 time_point 转换为 std::time_t 类型
    std::time_t currentTime = std::chrono::high_resolution_clock::to_time_t(timePoint);

    // 将时间转换为本地时间
    std::tm* localTime = std::localtime(&currentTime);

    // 将本地时间转换为字符串
    std::stringstream ss;
    // ss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    
    // 获取毫秒部分
    auto duration = timePoint.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;

    for (int i = 0; i < data_per_bag_; i++)
    {
        ss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
        // 添加毫秒部分到字符串
        ss << '.' << std::setw(3) << std::setfill('0') << milliseconds-5+i*0.5;

        time_stamp_.push(ss.str());

        // 清空ss以便接收新的数据
        ss.str("");
        ss.clear();
    }

}

// 单次接收函数
void UDPReceiver::receive_once(int data_number)
{
    // 创建文件 检查文件是否成功打开
    output_file_.open(file_name_, std::ios_base::app);
    if (!output_file_.is_open())
        std::cout << "-file open error" << std::endl;
    else
        std::cout << "-file is open" << std::endl;
    
    // 获取偏置
    // std::cout << "-start to get bias\n";
    // getBias();
    // std::cout << "-SUCCESS\n" << std::endl;

    receive_max = data_number;
    receive_loop_flag_ = 0;     // 单次
    receive_count_ = 0;         // 初始化接收计数器

    startReceive();

    // 开始接收
    std::cout << "start receive" << std::endl;
    io_context_.reset();
    io_context_.run();
    output_file_.close();
}

// 阻塞接收函数
void UDPReceiver::receive_loop()
{
    // 创建文件
    output_file_.open(file_name_, std::ios_base::app);
    if (!output_file_.is_open())
        std::cout << "-file open error" << std::endl;
    else
        std::cout << "-file is open\n" << std::endl;

    // std::cout << "-start to get bias\n";
    // getBias();
    // std::cout << "-SUCCESS\n" << std::endl;

    receive_loop_flag_ = 1;     // 循环
    receive_count_ = 0;

    // 配置异步控制器
    startReceive();
    
    // 开始接收
    std::cout << "start receive" << std::endl;
    io_context_.run();
    output_file_.close();
}

// 启动接收函数（新）
void UDPReceiver::startReceive_new(std::vector<std::queue<float>>& OutputData)
{
    std::size_t bytes_transferred = socket_.receive(asio::buffer(buffer_));
    std::string received_data(buffer_.data(), bytes_transferred);

    handleReceivedData(received_data);
    processRawData();

    OutputData = processed_data_;
    for (int i = 0; i < ChannelSize; i++)
    {
        while (!processed_data_[i].empty())
        {
            processed_data_[i].pop();
        }
    }
}

void UDPReceiver::startReceive_new(float output[4][20])
{
    std::size_t bytes_transferred = socket_.receive(asio::buffer(buffer_));
    std::string received_data(buffer_.data(), bytes_transferred);

    handleReceivedData(received_data);
    processRawData();

    for (int i = 0; i < ChannelSize; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            output[i][j] = processed_data_[i].front();
            processed_data_[i].pop();
        }
    }
}

// int UDPReceiver::startReceive_new_async(float** output)
// {
//     char * buffer;
//     socket_.async_receive(asio::buffer(buffer),
//         [this](std::size_t bytes_transferred)
//         {
//             handleReceive_new_async(bytes_transferred);
//         });

// }

// int UDPReceiver::handleReceive_new_async(std::size_t bytes_transferred)
// {
//     std::string received_data(buffer_.data(), bytes_transferred);

//     handleReceivedData(received_data);
//     processRawData();

//     output = processed_data_;

//     return 0;
// }

// // 解包数据到向量
// int UDPReceiver::handleReceivedData_new(std::string input, float **output)
// {
//    std::istringstream iss(input);  // 创建字符串流
//    std::string line;
//    data_per_bag_ = input.size()/50;

//    // std::cout << input.size() << std::endl;
//    for (int i = 0; i < data_per_bag_; i++)  // 按行读取字符串
//    {
//        std::getline(iss, line);
//        size_t startPos = line.find(":");   // 去除每行开头的"data: "部分

//        if (startPos != std::string::npos)  // 有效数据
//        {
//            line = line.substr(startPos + 2);
//            std::istringstream lineStream(line);  // 创建每行的字符串流
//            std::string value;
//            for (int j = 0; j < ChannelSize; j++)  // 按列处理数据
//            {
//                if (std::getline(lineStream, value, ','))  // 按逗号分割字符串
//                {
//                    float number = std::strtod(value.c_str(), nullptr);  // 将字符串转换为 float
//                    // std::cout << std::fixed << number << " ";    // *test
//                    raw_data_[j].push(number - bias_[j]);
//                }
//            }
//            // std::cout << std::endl;  // *test
//        }
//        else    // 无效数据
//        {
//            for (int j = 0; j < ChannelSize; j++)  // 按列处理数据
//            {
//                float number = raw_data_[j].back();
//                raw_data_[j].push(number - bias_[j]);
//            }
//        }
//    }
//    return 0;
// }

















