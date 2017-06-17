/*****************************************************************
Name : 
Date : 2017/05/22
By   : CharlotteHonG
Final: 2017/05/22

source: http://www.graphics.cornell.edu/~bjw/rgbe/
*****************************************************************/
#pragma warning(disable : 4996)
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <cmath>
#include "rgbe\rgbe_lib.h"
#include "Rgbe.hpp"
using namespace std;

//----------------------------------------------------------------
Rgbe::Rgbe(string name): file_name(name){
    if (!fopen(name.c_str(), "rb"))
        throw bad_openFile("# Error opening file.");
}
//----------------------------------------------------------------
void Rgbe::Read_HDR(){
    // 開檔
    FILE* HDR_File = fopen(file_name.c_str(), "rb");
    if (!HDR_File) throw bad_openFile("# Error opening file.");
    // 讀取
    RGBE_ReadHeader(HDR_File, &img_width, &img_height, NULL);
    HDR_pix.resize(img_width*img_height*3);
    RGBE_ReadPixels_RLE(HDR_File, HDR_pix.data(), img_width, img_height);
    fclose(HDR_File);
}
//----------------------------------------------------------------
void Rgbe::Info(){
    cout << "File_mname: " << file_name << endl;
    cout << "img_width : " << img_width << endl;
    cout << "img_height: " << img_height << endl;
}
string& Rgbe::Out_name(string& name, string ref=""){
    name += "_";
    name += to_string(img_width);
    name += "x";
    name += to_string(img_height);
    if(ref!="") {
        name += "_";
        name += ref;
    }
    name += ".raw";
    return name;
}
void Rgbe::Write_raw(vector<float>& pix, std::string name){
    vector<imch> RGB_pix;
    size_t len = pix.size();
    RGB_pix.resize(len);
    for(unsigned i = 0; i < len; ++i) {
        float temp = round(pix[i]*255);
        if(temp > 255) {RGB_pix[i] = imch(255);}
        else if(temp < 0) {RGB_pix[i] = imch(0);}
        else {RGB_pix[i] = static_cast<imch>(temp);}
    }
    FILE* HDR_File = fopen(name.c_str(), "wb");
    if (!HDR_File) throw bad_openFile("# Error opening file.");
    fwrite((char*)RGB_pix.data(), sizeof(imch), len, HDR_File);
    fclose(HDR_File);
}
//----------------------------------------------------------------
// RGB 轉 XYZ
void Rgbe::rgb2xyz(vector<float>& XYZ_pix, vector<float>& RGB_pix) {
    XYZ_pix.resize(RGB_pix.size());
    for(unsigned i = 0; i < RGB_pix.size()/3; ++i) {
        r3dim(XYZ_pix, i, 0)  = float(0.412453) * r3dim(RGB_pix, i, R);
        r3dim(XYZ_pix, i, 0) += float(0.357580) * r3dim(RGB_pix, i, G);
        r3dim(XYZ_pix, i, 0) += float(0.180423) * r3dim(RGB_pix, i, B);

        r3dim(XYZ_pix, i, 1)  = float(0.212671) * r3dim(RGB_pix, i, R);
        r3dim(XYZ_pix, i, 1) += float(0.715160) * r3dim(RGB_pix, i, G);
        r3dim(XYZ_pix, i, 1) += float(0.072169) * r3dim(RGB_pix, i, B);

        r3dim(XYZ_pix, i, 2)  = float(0.019334) * r3dim(RGB_pix, i, R);
        r3dim(XYZ_pix, i, 2) += float(0.119193) * r3dim(RGB_pix, i, G);
        r3dim(XYZ_pix, i, 2) += float(0.950227) * r3dim(RGB_pix, i, B);
    }
}
// XYZ 轉 Yxy
void Rgbe::xyz2Yxy(vector<float>& Yxy_pix, vector<float>& XYZ_pix) {
    Yxy_pix.resize(XYZ_pix.size());
    auto W = [&](size_t idx) {
        return r3dim(XYZ_pix, idx, 0) +
            r3dim(XYZ_pix, idx, 1)+
            r3dim(XYZ_pix, idx, 2);
    };
    for(unsigned i = 0; i < Yxy_pix.size()/3; ++i) {
        r3dim(Yxy_pix, i, 0) = r3dim(XYZ_pix, i, 1);
        r3dim(Yxy_pix, i, 1) = r3dim(XYZ_pix, i, 0) / W(i);
        r3dim(Yxy_pix, i, 2) = r3dim(XYZ_pix, i, 1) / W(i);
    }
}
// RGB 轉 Yxz
void Rgbe::rgb2Yxy(vector<float>& Yxy_pix, vector<float>& RGB_pix) {
    float a, b, c;
    for(unsigned i = 0; i < Yxy_pix.size()/3; ++i) {
        a = float(0.412453) * r3dim(RGB_pix, i, R)+
            float(0.357580) * r3dim(RGB_pix, i, G)+
            float(0.180423) * r3dim(RGB_pix, i, B);
        b = float(0.212671) * r3dim(RGB_pix, i, R)+
            float(0.715160) * r3dim(RGB_pix, i, G)+
            float(0.072169) * r3dim(RGB_pix, i, B);
        c = float(0.019334) * r3dim(RGB_pix, i, R)+
            float(0.119193) * r3dim(RGB_pix, i, G)+
            float(0.950227) * r3dim(RGB_pix, i, B);
        r3dim(Yxy_pix, i, 0) = b;
        r3dim(Yxy_pix, i, 1) = a / (a+b+c);
        r3dim(Yxy_pix, i, 2) = b / (a+b+c);
    }
}
void Rgbe::rgb2Yxy(vector<float>& RGB_pix){
    rgb2Yxy(RGB_pix, RGB_pix);
}
//----------------------------------------------------------------
// Yxz 轉 xyz
void Rgbe::Yxz2xyz(vector<float>& XYZ_pix, vector<float>& Yxy_pix){
    for(unsigned i = 0; i < Yxy_pix.size()/3; ++i){
        float newW = r3dim(Yxy_pix, i, 0) / r3dim(Yxy_pix, i, 2);
        r3dim(XYZ_pix, i, 1) = r3dim(Yxy_pix, i, 0);
        r3dim(XYZ_pix, i, 0) = newW * r3dim(Yxy_pix, i, 1);
        r3dim(XYZ_pix, i, 2) = newW -
            r3dim(XYZ_pix, i, 0) - r3dim(XYZ_pix, i, 1);
    }
}
// xyz 轉 rgb
void Rgbe::xyz2rgb(vector<float>& RGB_pix, vector<float>& XYZ_pix){
    for(unsigned i = 0; i < XYZ_pix.size()/3; ++i) {
        r3dim(RGB_pix, i, 0)  =  float(3.240479)*r3dim(XYZ_pix, i, 0);
        r3dim(RGB_pix, i, 0) += -float(1.537150)*r3dim(XYZ_pix, i, 1);
        r3dim(RGB_pix, i, 0) += -float(0.498535)*r3dim(XYZ_pix, i, 2);

        r3dim(RGB_pix, i, 1)  = -float(0.969256)*r3dim(XYZ_pix, i, 0);
        r3dim(RGB_pix, i, 1) +=  float(1.875992)*r3dim(XYZ_pix, i, 1);
        r3dim(RGB_pix, i, 1) +=  float(0.041556)*r3dim(XYZ_pix, i, 2);

        r3dim(RGB_pix, i, 2)  =  float(0.055648)*r3dim(XYZ_pix, i, 0);
        r3dim(RGB_pix, i, 2) += -float(0.204043)*r3dim(XYZ_pix, i, 1);
        r3dim(RGB_pix, i, 2) +=  float(1.057311)*r3dim(XYZ_pix, i, 2);
    }
}
// Yxz 轉 rgb
void Rgbe::Yxz2rgb(vector<float>& RGB_pix, vector<float>& Yxy_pix){
    vector<float> XYZ_pix(Yxy_pix.size());
    float a, b, c, newW;
    for(unsigned i = 0; i < XYZ_pix.size()/3; ++i) {
        newW = r3dim(Yxy_pix, i, 0) / r3dim(Yxy_pix, i, 2);
        a = r3dim(Yxy_pix, i, 0);
        b = newW * r3dim(Yxy_pix, i, 1);
        c = newW - b - a;

        r3dim(RGB_pix, i, 0)  =  float(3.240479)*b;
        r3dim(RGB_pix, i, 0) += -float(1.537150)*a;
        r3dim(RGB_pix, i, 0) += -float(0.498535)*c;

        r3dim(RGB_pix, i, 1)  = -float(0.969256)*b;
        r3dim(RGB_pix, i, 1) +=  float(1.875992)*a;
        r3dim(RGB_pix, i, 1) +=  float(0.041556)*c;

        r3dim(RGB_pix, i, 2)  =  float(0.055648)*b;
        r3dim(RGB_pix, i, 2) += -float(0.204043)*a;
        r3dim(RGB_pix, i, 2) +=  float(1.057311)*c;
    }
}
void Rgbe::Yxz2rgb(vector<float>& Yxy_pix){
    Yxz2rgb(Yxy_pix, Yxy_pix);
}
//----------------------------------------------------------------
