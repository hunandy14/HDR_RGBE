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
using namespace std;

//----------------------------------------------------------------

enum RGB {R, G, B};
class RGB_t {
public:
    RGB_t(size_t rgb): rgb(RGB(rgb)) {
        if(rgb>2) {throw range_error("range only 0~2");}
    }
    inline operator RGB() {return rgb;}
private:
    RGB rgb;
};
//----------------------------------------------------------------

using imch = unsigned char;
class Rgbe {
public:
    Rgbe(string name);
    virtual ~Rgbe() = default;
    float& at_HDR(size_t idx, RGB_t rgb);
    const float& at_HDR(size_t idx, RGB_t rgb) const;
public:
    static float& r3dim(vector<float>& pix, size_t idx, RGB_t rgb);
    static void rgb2xyz(vector<float>& XYZ_pix, vector<float>& RGB_pix);
    static void xyz2Yxy(vector<float>& Yxy_pix, vector<float>& XYZ_pix);
public:
    void Read_HDR();
public:
    size_t Canvas_Size();
    virtual void Info();
    string& Out_name(string& name, string ref);
    void Write_raw(vector<float>& Map_pix, string name, string bit="24");
protected:
    bool File_open(string name, string sta);
protected:
    string file_name;
    int img_width = 0;
    int img_height = 0;
protected:
    vector<float> HDR_pix;
};
//----------------------------------------------------------------

class Rgbe2Raw: public Rgbe {
public:
    Rgbe2Raw(string name): Rgbe(name) {}
    ~Rgbe2Raw() {}
    imch& at_RGB(size_t idx, RGB_t rgb);
    const imch& at_RGB(size_t idx, RGB_t rgb) const;
public:
    void Write_Raw(string name);
    void Write_Gray(string name);
private:
    vector<imch> RGB_pix;
    vector<imch> Gray_pix;
};
//----------------------------------------------------------------
class Rgbe_Mapping: public Rgbe{
public:
    Rgbe_Mapping(string name): Rgbe(name){}
    ~Rgbe_Mapping() {}
    float& at_Map(size_t idx, RGB_t rgb);
    const float& at_Map(size_t idx, RGB_t rgb) const;
public:
    void rgb_Map();
public:
    vector<float> Yxy_pix;
    void rgb_Map3(float dmax=100, float b=0.85);
    static auto Mapping(vector<float> pix, 
        float dmax=100, float b=0.85)-> decltype(pix);
public:
    void rgb_Map2(float dmax=100, float b=0.85);
    vector<float> gray_pix;
    vector<float> R_pix;
public:
    vector<float> Map_pix;
};
