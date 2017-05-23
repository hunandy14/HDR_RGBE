/*****************************************************************
Name : 
Date : 2017/05/23
By   : CharlotteHonG
Final: 2017/05/23
*****************************************************************/
#pragma warning(disable : 4996)
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include "Rgbe.hpp"
using namespace std;

void Rgbe2Raw::Write_Raw(string name){
    size_t len = Canvas_Size()*3;
    RGB_pix.resize(len);
    for(unsigned i = 0; i < len; ++i) {
        float temp = round(HDR_pix[i]*255);
        if(temp > 255) {RGB_pix[i] = imch(255);}
        else if(temp < 0) {RGB_pix[i] = imch(0);}
        else {RGB_pix[i] = static_cast<imch>(temp);}
    }

    Out_name(name, "24bit");
    File_open(name, "wb");
    FILE* HDR_File = fopen(name.c_str(), "wb");
    fwrite((char*)RGB_pix.data(), sizeof(imch), len, HDR_File);
    fclose(HDR_File);
}
void Rgbe2Raw::Write_Gray(string name){
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