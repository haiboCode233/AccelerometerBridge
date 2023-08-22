#ifndef FILTER_H
#define FILTER_H

#include <stdio.h>
#include <math.h>
#include <vector>
#include <queue>
#include <iostream>

#define PI acos(-1)

class Filter
{
public:
    // 构造函数
    Filter();
    
    // 滤波器系数生成函数
    std::vector <float> sinc(std::vector<float> x);
    std::vector <float> fir1(int n, std::vector<float> Wn);   // 类似于 matlab 中的 fir1 函数

    // 生成滤波器
    std::vector<float> generate_Filter();

    // 一维卷积函数    
    std::vector<float> fir;

    float order;
    float freq_s;
    float freq_c;

private:



};

#endif
