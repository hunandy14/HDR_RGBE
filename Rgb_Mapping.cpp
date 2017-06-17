/*****************************************************************
Name :
Date : 2017/05/31
By   : CharlotteHonG
Final: 2017/05/31
*****************************************************************/
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include "rgbe\rgbe_lib.h"
#include "Rgbe.hpp"
using namespace std;
using class_t = Rgbe_Map;

// Yxy 模型的Y 做映射
void class_t::Map(float dmax, float b) {
    // 長度
    size_t len = HDR_pix.size()/3;
    Map_pix.resize(len*3);
    // RGB 轉 Yxy
    rgb2Yxy(Map_pix, HDR_pix);
    // 取出Y
    vector<float> pix(len);
    for(unsigned i = 0; i < len; ++i)
        pix[i] = r3dim(Map_pix, i, 0);
    // 單維度色調映射
    Mapping(pix, dmax, b);
    for(unsigned i = 0; i < len; ++i){
        r3dim(Map_pix, i, 0) = pix[i];
    }
    // Yxz 轉 rgb
    Yxz2rgb(Map_pix);
    // gama 校正
    gama_fix(Map_pix, 2.7);
    // 輸出檔案
    string name = "Yxz_Map";
    name += "_dmax" + to_string(int(dmax));
    name += "_b" + to_string(b).substr(0, 4);

    Write_raw(Map_pix, Out_name(name, "24bit"));
}
// 映射
void class_t::Mapping(vector<float>& lumi, float dmax, float b){
    float maxLum = *std::max_element(lumi.begin(), lumi.end());
    size_t N = lumi.size();
    float logSum = 0;
    for(unsigned i = 0; i < N; ++i)
        logSum += log(lumi[i]);
    float logAvgLum = logSum/N;
    float avgLum = exp(logAvgLum);
    float maxLumW = (maxLum / avgLum);
    float coeff = (dmax*float(0.01)) / log10(maxLumW+1);
    for(unsigned i = 0; i < N; ++i){
        lumi[i] /= avgLum;
        lumi[i] = log(lumi[i]+1) /
            log(2 + pow((lumi[i]/maxLumW),(log(b)/log(0.5)))*8);
        lumi[i] *= coeff;
    }
}
// gama校正
void class_t::gama_fix(vector<float>& RGB_pix, float gamma){
    float slope = 4.5;
    float start = 0.018;
    float fgamma = (0.45/gamma)*2;
    // 判定係數
    if (gamma >= float(2.1)){
        start /= ((gamma - 2) * float(7.5));
        slope *= ((gamma - 2) * float(7.5));
    } else if (gamma <= float(1.9)){
        start *= ((2 - gamma) * float(7.5));
        slope /= ((2 - gamma) * float(7.5));
    }
    // 校正像素
    for(auto&& i : RGB_pix) {
        if (i <= start){
            i = i*slope;
        } else {
            i = float(1.099)*pow(i, fgamma) - float(0.099);
        }
    }
}
//----------------------------------------------------------------
//----------------------------------------------------------------
