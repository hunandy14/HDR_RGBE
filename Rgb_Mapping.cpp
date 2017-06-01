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
void class_t::rgb_Map(){
    float max[3]{at_HDR(0, R), at_HDR(0, G), at_HDR(0, B)};
    for(unsigned i = 1; i < HDR_pix.size()/3; ++i) {
        if(at_HDR(i, R) > max[R])
            max[R] = at_HDR(i, R);
        if(at_HDR(i, G) > max[G])
            max[G] = at_HDR(i, G);
        if(at_HDR(i, B) > max[B])
            max[B] = at_HDR(i, B);
    }

    Map_pix.resize(HDR_pix.size());
    constexpr float dmax = 255;
    constexpr float b = 0.85;

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

    string name = "Mapping_dmax255_";
    vector<imch> RGB_pix;
    size_t len = Canvas_Size()*3;
    RGB_pix.resize(len);
    for(unsigned i = 0; i < len; ++i) {
        float temp = round(Map_pix[i]*255);
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




float& class_t::at_Map(size_t idx, RGB_t rgb){
    return const_cast<float&>(
        static_cast<const class_t&>(*this).at_Map(idx, (rgb)));
}
const float& class_t::at_Map(size_t idx, RGB_t rgb) const {
    return Map_pix[(idx*3)+rgb];
}