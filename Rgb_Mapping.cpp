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
using class_t = Rgbe_Mapping;

// Yxy 模型的Y 做映射
void class_t::rgb_Map3(float dmax, float b) {
    // 長度
    size_t len = HDR_pix.size()/3;
    // RGB 轉 Yxy
    vector<float> Yxy_pix(len*3);
    rgb2Yxy(Yxy_pix, HDR_pix);
    // 取出Y
    vector<float> pix(len);
    for(unsigned i = 0; i < len; ++i)
        pix[i] = r3dim(Yxy_pix, i, 0);
    // 單維度色調映射
    Mapping(pix, dmax, b);
    for(unsigned i = 0; i < len; ++i){
        r3dim(Yxy_pix, i, 0) = pix[i];
    }
    // Yxz 轉 rgb
    Map_pix.resize(len*3);
    Yxz2rgb(Map_pix, Yxy_pix);


    for(unsigned i = 0; i < 10; ++i) {
        cout << at_Map(i, 0) << endl;
    }

    // 輸出檔案
    string name = "Yxz_Map";
    name += "_dmax" + to_string(int(dmax));
    name += "_b" + to_string(b).substr(0, 4);
    Write_raw(Map_pix, name);
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

void class_t::gama_fix(){
    
}
// 其他映射
#include "Rgb_Mapping2.cpp"
//----------------------------------------------------------------
inline
float& class_t::at_Map(size_t idx, RGB_t rgb) {
    return const_cast<float&>(
               static_cast<const class_t&>(*this).at_Map(idx, (rgb)));
}
inline
const float& class_t::at_Map(size_t idx, RGB_t rgb) const {
    return Map_pix[(idx*3)+rgb];
}
//----------------------------------------------------------------
