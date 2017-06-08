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
    size_t len = HDR_pix.size()/3;
    // RGB 轉 XYZ
    vector<float> XYZ_pix(len*3);
    // rgb2xyz(XYZ_pix, HDR_pix);
    // XYZ 轉 Yxy
    vector<float> Yxy_pix(len*3);
    // xyz2Yxy(Yxy_pix, XYZ_pix);

    // RGB 轉 Yxy
    rgb2Yxy(Yxy_pix, HDR_pix);

    vector<float> pix(len);
    // 取出Y
    for(unsigned i = 0; i < len; ++i)
        pix[i] = r3dim(Yxy_pix, i, 0);
    // 單維度色調映射
    Mapping(pix, dmax, b);

    // Yxz 轉 xyz
    vector<float> newW(len);
    for(unsigned i = 0; i < len; ++i){
        newW[i] = pix[i] / r3dim(Yxy_pix, i, 2);
        r3dim(XYZ_pix, i, 1) = pix[i];
        r3dim(XYZ_pix, i, 0) = newW[i] * r3dim(Yxy_pix, i, 1);
        r3dim(XYZ_pix, i, 2) = newW[i] -
            r3dim(XYZ_pix, i, 0) - r3dim(XYZ_pix, i, 1);
    }
    // xyz 轉 RGB
    Map_pix.resize(len*3);
    for(unsigned i = 0; i < len; ++i) {
        r3dim(Map_pix, i, 0)  =  3.240479*r3dim(XYZ_pix, i, 0);
        r3dim(Map_pix, i, 0) += -1.537150*r3dim(XYZ_pix, i, 1);
        r3dim(Map_pix, i, 0) += -0.498535*r3dim(XYZ_pix, i, 2);

        r3dim(Map_pix, i, 1)  = -0.969256*r3dim(XYZ_pix, i, 0);
        r3dim(Map_pix, i, 1) +=  1.875992*r3dim(XYZ_pix, i, 1);
        r3dim(Map_pix, i, 1) +=  0.041556*r3dim(XYZ_pix, i, 2);

        r3dim(Map_pix, i, 2)  =  0.055648*r3dim(XYZ_pix, i, 0);
        r3dim(Map_pix, i, 2) += -0.204043*r3dim(XYZ_pix, i, 1);
        r3dim(Map_pix, i, 2) +=  1.057311*r3dim(XYZ_pix, i, 2);
    }

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
void class_t::Mapping(vector<float>& pix, float dmax, float b){
    float maxLum = *std::max_element(pix.begin(), pix.end());
    size_t N = pix.size();
    float logSum = 0;
    for(unsigned i = 0; i < N; ++i)
        logSum += log(pix[i]);
    float logAvgLum = logSum/N;
    float avgLum = exp(logAvgLum);
    float maxLumW = (maxLum / avgLum);
    float coeff = (dmax*float(0.01)) / log10(maxLumW+1);
    for(unsigned i = 0; i < N; ++i){
        pix[i] /= avgLum;
        pix[i] = log(pix[i]+1) /
            log(2 + pow((pix[i]/maxLumW),(log(b)/log(0.5)))*8);
        pix[i] *= coeff;
    }
}
// 轉灰階在做映射在轉彩色
void class_t::rgb_Map2(float dmax, float b) {
    // 轉灰階
    auto rgbmax = [&](float a, float b, float c) {
        float max = std::max(a, b);
        return std::max(max, c);
    };
    gray_pix.resize(HDR_pix.size()/3);
    for(unsigned i = 1; i < HDR_pix.size()/3; ++i) {
        gray_pix[i] = rgbmax(
                          HDR_pix[i*3+R],
                          HDR_pix[i*3+G],
                          HDR_pix[i*3+B]
                      );
    }
    // 轉 I (沒有亮度的ˊ彩色)
    R_pix.resize(HDR_pix.size());
    for(unsigned i = 1; i < HDR_pix.size()/3; ++i) {
        R_pix[i*3+R] = HDR_pix[i*3+R]/gray_pix[i];
        R_pix[i*3+G] = HDR_pix[i*3+G]/gray_pix[i];
        R_pix[i*3+B] = HDR_pix[i*3+B]/gray_pix[i];
    }
    // 參數
    double gmax = *std::max_element(gray_pix.begin(), gray_pix.end());
    Map_pix.resize(HDR_pix.size());
    // 對灰階做映射
    auto mapping = [&](size_t idx) {
        return
            dmax*0.01 * log(gray_pix[idx]+1.0) / (
                log10(gmax+1) *
                log(2+(
                        pow((gray_pix[idx]/gmax),
                            (log(b)/log(0.5)))
                    )*8
                   )
            );
    };
    for(unsigned i = 0; i < HDR_pix.size()/3; ++i) {
        float I = mapping(i);
        // float I = gray_pix[i];
        Map_pix[i*3 + R] = I * R_pix[i*3+R];
        Map_pix[i*3 + G] = I * R_pix[i*3+G];
        Map_pix[i*3 + B] = I * R_pix[i*3+B];
    }
    string name = "grayMapping_dmax";
    name += to_string(int(dmax));
    name += "_";
    Write_raw(HDR_pix, name);
}
// 直接做沒有任何處理
void class_t::rgb_Map() {
    // 最大值
    float max[3] {at_HDR(0, R), at_HDR(0, G), at_HDR(0, B)};
    for(unsigned i = 1; i < HDR_pix.size()/3; ++i) {
        if(at_HDR(i, R) > max[R])
            max[R] = at_HDR(i, R);
        if(at_HDR(i, G) > max[G])
            max[G] = at_HDR(i, G);
        if(at_HDR(i, B) > max[B])
            max[B] = at_HDR(i, B);
    }
    // 參數
    Map_pix.resize(HDR_pix.size());
    constexpr float dmax = 100;
    constexpr float b = 0.85;
    // 色調映射
    auto mapping = [&](size_t idx, RGB rgb) {
        return
            dmax*0.01 * log(at_HDR(idx, rgb)+1) / (
                log10(max[rgb]+1) *
                log(2+(
                        pow((at_HDR(idx, rgb)/max[rgb]),
                            (log(b)/log(0.5)))
                    )*8
                   )
            );
    };
    for(unsigned i = 0; i < HDR_pix.size()/3; ++i) {
        Map_pix[i*3 + R] = mapping(i, R);
        Map_pix[i*3 + G] = mapping(i, G);
        Map_pix[i*3 + B] = mapping(i, B);
    }
    string name = "Mapping_dmax";
    name += to_string(int(dmax));
    name += "_";
    Write_raw(Map_pix, name);
}
//----------------------------------------------------------------
float& class_t::at_Map(size_t idx, RGB_t rgb) {
    return const_cast<float&>(
               static_cast<const class_t&>(*this).at_Map(idx, (rgb)));
}
const float& class_t::at_Map(size_t idx, RGB_t rgb) const {
    return Map_pix[(idx*3)+rgb];
}
//----------------------------------------------------------------
