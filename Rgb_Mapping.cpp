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

    // 測試
    for(unsigned i = 0; i < 10; ++i) {
        cout << r3dim(Map_pix, i, 0) << endl;
    }
    gama_fix(Map_pix, 2.7);

    cout << "=========" << endl;
    // 測試
    for(unsigned i = 0; i < 10; ++i) {
        cout << r3dim(Map_pix, i, 0) << endl;
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
void class_t::gama_fix(vector<float>& RGB_pix, float gamma){
    float slope = 4.5;
    float start = 0.018;
    float fgamma = (0.45/gamma)*2;
    float num = 7.5;
    if (gamma >= float(2.1)){
        start /= ((gamma - 2) * num);
        slope *= ((gamma - 2) * num);
    }
    else if (gamma <= float(1.9)){
        start *= ((2 - gamma) * num);
        slope /= ((2 - gamma) * num);
    }
    for(auto&& i : RGB_pix) {
        if (i <= start){
            i = i*slope;
        } else {
            i = float(1.099)*pow(i, fgamma) - float(0.099);
        }
    }
    // 單點方式
    // for(unsigned i = 0; i < RGB_pix.size(); ++i) {
    //     if (RGB_pix[i] <= start){
    //         RGB_pix[i] = RGB_pix[i]*slope;
    //     } else {
    //         RGB_pix[i] = float(1.099)*pow(RGB_pix[i], fgamma) - float(0.099);
    //     }
    // }
    // 原始方式
    // for(auto row = 0; row < img_height; ++row) {
    //     for(auto col = 0; col < img_width; ++col) {
    //         // red
    //         if (r2d3dim(RGB_pix, row,col,0) <= start){
    //             r2d3dim(RGB_pix, row, col, 0) = r2d3dim(RGB_pix, row,col,0) * slope;
    //         } else {
    //             r2d3dim(RGB_pix, row, col, 0) =
    //                 float(1.099)*pow(r2d3dim(RGB_pix, row,col,0), fgamma) - float(0.099);
    //         }
    //         // green
    //         if (r2d3dim(RGB_pix, row,col,1) <= start){
    //             r2d3dim(RGB_pix, row, col, 1) = r2d3dim(RGB_pix, row,col,1) * slope;
    //         } else {
    //             r2d3dim(RGB_pix, row, col, 1) =
    //                 float(1.099)*pow(r2d3dim(RGB_pix, row,col,1), fgamma) - float(0.099);
    //         }
            
    //         // blue
    //         if (r2d3dim(RGB_pix, row,col,2) <= start){
    //             r2d3dim(RGB_pix, row, col, 2) = r2d3dim(RGB_pix, row,col,2) * slope;
    //         } else {
    //             r2d3dim(RGB_pix, row, col, 2) =
    //                 float(1.099)*pow(r2d3dim(RGB_pix, row,col,2), fgamma) - float(0.099);
    //         }
    //     }
    // }
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
