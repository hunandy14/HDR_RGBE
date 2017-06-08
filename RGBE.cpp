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
    File_open(file_name+=".hdr", "rb");
}
//----------------------------------------------------------------
void Rgbe::Read_HDR(){
    FILE* HDR_File;
    HDR_File = fopen(file_name.c_str(), "rb");
    RGBE_ReadHeader(HDR_File, &img_width, &img_height, NULL);
    // Info();
    HDR_pix.resize(Canvas_Size()*3);
    RGBE_ReadPixels_RLE(HDR_File, HDR_pix.data(), img_width, img_height);
   fclose(HDR_File);
}
//----------------------------------------------------------------
size_t Rgbe::Canvas_Size(){
    return img_width*img_height;
}
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
void Rgbe::Write_raw(vector<float>& pix, string name, string bit){
    vector<imch> RGB_pix;
    size_t len = pix.size();
    RGB_pix.resize(len);
    for(unsigned i = 0; i < len; ++i) {
        float temp = round(pix[i]*255);
        if(temp > 255) {RGB_pix[i] = imch(255);}
        else if(temp < 0) {RGB_pix[i] = imch(0);}
        else {RGB_pix[i] = static_cast<imch>(temp);}
    }
    Out_name(name, bit+"bit");
    File_open(name, "wb");
    FILE* HDR_File = fopen(name.c_str(), "wb");
    fwrite((char*)RGB_pix.data(), sizeof(imch), len, HDR_File);
    fclose(HDR_File);
}
//----------------------------------------------------------------
bool Rgbe::File_open(string name, string sta){
    FILE* HDR_File;
    if (!(HDR_File = fopen(name.c_str(), sta.c_str()))){
        cout << "# Can't open the [" << name << "]!!" << endl;
        exit(0);
    } fclose(HDR_File);
    return 1;
}
//----------------------------------------------------------------
inline
float& Rgbe::at_HDR(size_t idx, RGB_t rgb){
    return const_cast<float&>(
        static_cast<const Rgbe&>(*this).at_HDR(idx, (rgb)));
}
inline
const float& Rgbe::at_HDR(size_t idx, RGB_t rgb) const {
    return HDR_pix[(idx*3)+rgb];
}
inline
float& Rgbe::r3dim(vector<float>& pix, size_t idx, RGB_t rgb){
    return pix[(idx*3)+rgb];
}
//----------------------------------------------------------------
// RGB 轉 XYZ
void Rgbe::rgb2xyz(vector<float>& XYZ_pix, vector<float>& RGB_pix) {
    XYZ_pix.resize(RGB_pix.size());
    // 轉XYZ模型
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
// RGB 轉 YXY
void Rgbe::xyz2Yxy(vector<float>& Yxy_pix, vector<float>& XYZ_pix) {
    auto W = [&](size_t idx) {
        return r3dim(XYZ_pix, idx, 0) +
            r3dim(XYZ_pix, idx, 1)+
            r3dim(XYZ_pix, idx, 2);
    };
    // 轉模型
    Yxy_pix.resize(XYZ_pix.size());
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
//----------------------------------------------------------------
