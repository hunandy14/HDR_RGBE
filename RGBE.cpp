/*****************************************************************
Name : 
Date : 2017/05/22
By   : CharlotteHonG
Final: 2017/05/22

source: http://www.graphics.cornell.edu/~bjw/rgbe/
*****************************************************************/
#pragma warning(disable : 4996)
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include "rgbe\rgbe.h"
#include "rgbe\rgbe.c"
using namespace std;

class RGBE{
public:
    RGBE(string name): file_name(name){
        File_open(file_name+=".hdr", "rb");
    }
    void Read_HDR(){
        FILE* HDR_File;
        HDR_File = fopen(file_name.c_str(), "rb");
        RGBE_ReadHeader(HDR_File, &img_width, &img_height, NULL);
        // Info();
        HDR_pix.resize(Canvas_Size()*3);
        RGBE_ReadPixels_RLE(HDR_File, HDR_pix.data(), img_width, img_height);
        fclose(HDR_File);
    }
    void Write_Raw(string name){
        name += "_";
        name += to_string(img_width);
        name += "x";
        name += to_string(img_height);
        name += "_24bit.raw";

        size_t len = Canvas_Size()*3;
        Raw_pix.resize(len);
        for(unsigned i = 0; i < len; ++i) {
            float temp = round(HDR_pix[i]*255);
            if(temp > 255) {Raw_pix[i] = char(255);}
            else if(temp < 0) {Raw_pix[i] = char(0);}
            else {Raw_pix[i] = static_cast<char>(temp);}
        }
        
        File_open(name, "wb");
        FILE* HDR_File = fopen(name.c_str(), "wb");
        fwrite(Raw_pix.data(), sizeof(char), len, HDR_File);
        fclose(HDR_File);
    }
public:
    inline size_t Canvas_Size(){
        return img_width*img_height;
    }
    void Info(){
        cout << "File_mname: " << file_name << endl;
        cout << "img_width : " << img_width << endl;
        cout << "img_height: " << img_height << endl;
    }
private:
    bool File_open(string name, string sta){
        FILE* HDR_File;
        if (!(HDR_File = fopen(name.c_str(), sta.c_str()))){
            cout << "# Can't open the [" << name << "]!!" << endl;
            exit(0);
        } fclose(HDR_File);
        return 1;
    }
private:
    string file_name;
    int img_width = 0;
    int img_height = 0;
    vector<float> HDR_pix;
    vector<char> Raw_pix;
};