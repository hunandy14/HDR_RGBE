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
void class_t::Map(float dmax, float b, float gam) {
    // 長度
    size_t len = HDR_pix.size()/3;
    Map_pix.resize(len*3);
    // RGB 轉 Yxy
    rgb2Yxy(Map_pix, HDR_pix);
    // 映射
    Mapping3dim(Map_pix, 0);
    // Yxz 轉 rgb
    Yxz2rgb(Map_pix);
    // gama 校正
    gama_fix(Map_pix, gam);
    // 儲存參數
    para[0]=dmax, para[1]=b, para[2]=gam;
}
//----------------------------------------------------------------
// 映射三維
void class_t::Mapping3dim(vector<float>& lumi_map, RGB_t rgb_map,
        float dmax, float b)
{
    Mapping3dim(lumi_map, rgb_map, lumi_map, rgb_map, dmax, b);
}
void class_t::Mapping3dim(vector<float>& lumi_map, RGB_t rgb_map,
        vector<float>& lumi, RGB_t rgb, 
        float dmax, float b)
{
    Mapping_basic(3, lumi_map, rgb_map, lumi, rgb, dmax, b);
}
// 映射一維
void class_t::Mapping(vector<float>& lumi, float dmax, float b){
    Mapping(lumi, lumi, dmax, b);
}
void class_t::Mapping(vector<float>& lumi_map,
    vector<float>& lumi, float dmax, float b)
{
    Mapping_basic(1, lumi_map, 0, lumi, 0, dmax, b);
}
//----------------------------------------------------------------
// 映射
void class_t::Mapping_basic(size_t dim, vector<float>& lumi_map, RGB_t rgb_map,
        vector<float>& lumi, RGB_t rgb, 
        float dmax, float b)
{
    float maxLum = lumi[0*dim+rgb];
    for(unsigned i = 1; i < lumi.size()/dim; ++i) {
        if(lumi[i*dim+rgb] > maxLum)
            maxLum = lumi[i*dim+rgb];
    }
    size_t N = lumi.size()/dim;
    float logSum = 0;
    for(unsigned i = 0; i < N; ++i)
        logSum += log(lumi[i*dim+rgb]);
    float logAvgLum = logSum/N;
    float avgLum = exp(logAvgLum);
    float maxLumW = (maxLum / avgLum);
    float coeff = (dmax*float(0.01)) / log10(maxLumW+1);
    for(unsigned i = 0; i < N; ++i) {
        lumi_map[i*dim+rgb] /= avgLum;
        lumi_map[i*dim+rgb] = log(lumi[i*dim+rgb]+1) /
                  log(2 + pow((lumi[i*dim+rgb]/maxLumW),(log(b)/log(0.5)))*8);
        lumi_map[i*dim+rgb] *= coeff;
    }
}
// gama校正
void class_t::gama_fix(vector<float>& RGB_pix, float gam) {
    float slope = 4.5;
    float start = 0.018;
    float fgamma = (0.45/gam)*2;
    // 判定係數
    if(gam >= float(2.1)) {
        start /= ((gam - 2) * float(7.5));
        slope *= ((gam - 2) * float(7.5));
    } else if(gam <= float(1.9)) {
        start *= ((2 - gam) * float(7.5));
        slope /= ((2 - gam) * float(7.5));
    }
    // 校正像素
    for(auto&& i : RGB_pix) {
        if(i <= start) {
            i = i*slope;
        } else {
            i = float(1.099)*pow(i, fgamma) - float(0.099);
        }
    }
}
//----------------------------------------------------------------
string class_t::Out_name(string name, string ref) {
    name += "_dmax" + to_string(int(para[0]));
    name += "_b" + to_string(para[1]).substr(0, 4);
    name += "_gam" + to_string(para[2]).substr(0, 3);
    return Rgbe::Out_name(name, ref);
}
void class_t::write(string name) {
    Write_raw(Map_pix, Out_name(name, "24bit")+".raw");
}
//----------------------------------------------------------------
