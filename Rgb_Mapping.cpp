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

// RGB 轉 XYZ
auto class_t::rgb2xyz() {
    // 轉XYZ模型
    XYZ_pix.resize(HDR_pix.size());
    for(unsigned i = 0; i < HDR_pix.size()/3; ++i) {
        r3dim(XYZ_pix, i, 0) += at_HDR(i, R)*float(0.412453);
        r3dim(XYZ_pix, i, 0) += at_HDR(i, G)*float(0.357580);
        r3dim(XYZ_pix, i, 0) += at_HDR(i, B)*float(0.180423);

        r3dim(XYZ_pix, i, 1) += at_HDR(i, R)*float(0.212671);
        r3dim(XYZ_pix, i, 1) += at_HDR(i, G)*float(0.715160);
        r3dim(XYZ_pix, i, 1) += at_HDR(i, B)*float(0.072169);

        r3dim(XYZ_pix, i, 2) += at_HDR(i, R)*float(0.019334);
        r3dim(XYZ_pix, i, 2) += at_HDR(i, G)*float(0.119193);
        r3dim(XYZ_pix, i, 2) += at_HDR(i, B)*float(0.950227);
    }
    return XYZ_pix;
}
// RGB 轉 YXY
auto class_t::rgb2Yxy() {
    // vector<float> Yxy_pix(XYZ_pix.size());
    // 獲取W
    vector<double> W(XYZ_pix.size()/3);
    for(unsigned i = 0; i < W.size(); ++i) {
        W[i] += r3dim(XYZ_pix, i, 0);
        W[i] += r3dim(XYZ_pix, i, 1);
        W[i] += r3dim(XYZ_pix, i, 2);
    }
    // 轉模型
    Yxy_pix.resize(XYZ_pix.size());
    for(unsigned i = 0; i < Yxy_pix.size()/3; ++i) {
        r3dim(Yxy_pix, i, 0) = r3dim(XYZ_pix, i, 1);
        r3dim(Yxy_pix, i, 1) = r3dim(XYZ_pix, i, 0) / W[i];
        r3dim(Yxy_pix, i, 2) = r3dim(XYZ_pix, i, 1) / W[i];
    }
    return Yxy_pix;
}
// Yxy 模型的Y 做映射
void class_t::rgb_Map3(float dmax, float b) {
    rgb2xyz();
    rgb2Yxy();
    size_t len = Yxy_pix.size()/3;
    vector<float> pix(len);
    // 取出Y
    for(unsigned i = 0; i < len; ++i)
        pix[i] = r3dim(Yxy_pix, i, 0);
    // 單維度色調映射
    pix = Mapping(pix, dmax, b);
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
        at_Map(i, 0) = 3.240479*r3dim(XYZ_pix, i, 0) + -1.537150*r3dim(XYZ_pix, i, 1) + -0.498535*r3dim(XYZ_pix, i, 2);
        at_Map(i, 1) =-0.969256*r3dim(XYZ_pix, i, 0) +  1.875992*r3dim(XYZ_pix, i, 1) +  0.041556*r3dim(XYZ_pix, i, 2);
        at_Map(i, 2) = 0.055648*r3dim(XYZ_pix, i, 0) + -0.204043*r3dim(XYZ_pix, i, 1) +  1.057311*r3dim(XYZ_pix, i, 2);
    }

    string name = "Yxz_Map";
    name += "_dmax" + to_string(int(dmax));
    name += "_b" + to_string(b).substr(0, 4);
    Write_raw(Map_pix, name);
}
// 映射
auto class_t::Mapping(vector<float> pix, float dmax, float b) -> decltype(pix){
    float maxLum = *std::max_element(pix.begin(), pix.end());
    size_t N = pix.size();
    float logSum = 0;
    for(unsigned i = 0; i < N; ++i)
        logSum += log(pix[i]);
    float logAvgLum = logSum/N;
    float avgLum = exp(logAvgLum);
    float maxLumW = (maxLum / avgLum);
    float coeff = (dmax*float(0.01)) / log10(maxLumW+1);
    for(unsigned i = 0; i < N; ++i)
        pix[i] /= avgLum;
    for(unsigned i = 0; i < N; ++i){
        pix[i] = log(pix[i]+1) /
            log(2 + pow((pix[i]/maxLumW),(log(b)/log(0.5)))*8);
        pix[i] *= coeff;
    }
    return pix;
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
