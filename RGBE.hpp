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
#include "rgbe\rgbe.h"
#include "rgbe\rgbe.c"
using namespace std;

enum RGB{R, G, B};
class RGB_t{
public:
    RGB_t(size_t rgb): rgb(RGB(rgb)){
        if(rgb>2) {throw range_error("range only 0~2");}
    }
    inline operator RGB(){return rgb;}
private:
    RGB rgb;
};

using imch = unsigned char;
class Rgbe{
public:
    Rgbe(string name);
    inline imch& at_RGB(size_t idx, RGB_t rgb);
    inline const imch& at_RGB(size_t idx, RGB_t rgb) const;
public:
    void Read_HDR();
    void Write_Raw(string name);
    void Gray(string name);
public:
    inline size_t Canvas_Size();
    void Info();
    string& Out_name(string& name, string ref);
private:
    bool File_open(string name, string sta);
private:
    string file_name;
    int img_width = 0;
    int img_height = 0;
    vector<float> HDR_pix;
    vector<imch> RGB_pix;
    vector<imch> Gray_pix;
};

class Rgbe2Raw{
public:
    Rgbe2Raw();
    ~Rgbe2Raw();
    
};