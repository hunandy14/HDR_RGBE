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
#include "RGBE.hpp"
using namespace std;

Rgbe::Rgbe(string name): file_name(name){
    File_open(file_name+=".hdr", "rb");
}
void Rgbe::Read_HDR(){
    FILE* HDR_File;
    HDR_File = fopen(file_name.c_str(), "rb");
    RGBE_ReadHeader(HDR_File, &img_width, &img_height, NULL);
    // Info();
    HDR_pix.resize(Canvas_Size()*3);
    RGBE_ReadPixels_RLE(HDR_File, HDR_pix.data(), img_width, img_height);
    fclose(HDR_File);
}
void Rgbe::Write_Raw(string name){
    size_t len = Canvas_Size()*3;
    RGB_pix.resize(len);
    for(unsigned i = 0; i < len; ++i) {
        float temp = round(HDR_pix[i]*255);
        if(temp > 255) {RGB_pix[i] = imch(255);}
        else if(temp < 0) {RGB_pix[i] = imch(0);}
        else {RGB_pix[i] = static_cast<imch>(temp);}
    }

    File_open(name, "wb");
    Out_name(name, "24bit");
    FILE* HDR_File = fopen(name.c_str(), "wb");
    fwrite((char*)RGB_pix.data(), sizeof(imch), len, HDR_File);
    fclose(HDR_File);
}
void Rgbe::Gray(string name){
    size_t len = Canvas_Size();
    Gray_pix.resize(len);
    for(unsigned i = 0; i < len; ++i) {
        Gray_pix[i] = (
            at_RGB(i, R)*19595 + 
            at_RGB(i, G)*38469 + 
            at_RGB(i, B)*7472) >> 16;
    }
    File_open(name, "wb");
    Out_name(name, "8bit");
    FILE* HDR_File = fopen(name.c_str(), "wb");
    fwrite((char*)Gray_pix.data(), sizeof(imch), len, HDR_File);
    fclose(HDR_File);
}
//----------------------------------------------------------------
inline size_t Rgbe::Canvas_Size(){
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
inline imch& Rgbe::at_RGB(size_t idx, RGB_t rgb){
    return const_cast<imch&>(
        static_cast<const Rgbe&>(*this).at_RGB(idx, (rgb)));
}
inline const imch& Rgbe::at_RGB(size_t idx, RGB_t rgb) const {
    return RGB_pix[(idx*3)+rgb];
}
