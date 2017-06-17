/*****************************************************************
Name : 
Date : 2017/05/22
By   : CharlotteHonG
Final: 2017/05/22
*****************************************************************/
#pragma warning(disable : 4996)
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include "Rgbe.hpp"
#include "Raw2Img/Raw2img.hpp"
// #include "Single_file.hpp"

using namespace std;

constexpr char img_name[]("HDRFile/seymour_park");
constexpr char raw_name[]("seymour_park");

clock_t start;
inline void pri_time(clock_t start, clock_t end){
    cout << "Runing Time : "
     << (double)(end-start) / CLOCKS_PER_SEC
     << " seconds" << endl;
}
//================================================================
int main(int argc, char const *argv[]){
    Rgbe_Map img(img_name);
    img.Read_HDR();
    img.Info();
    // 色調映射
    start = clock();
    img.Map(100, 0.85);
    pri_time(start, clock());

    // 輸出 BMP
    vector<float>& pix = img;
    vector<unsigned char> RGB_pix;
    size_t len = pix.size();
    RGB_pix.resize(len);
    for(unsigned i = 0; i < len; ++i) {
        float temp = round(pix[i]*255);
        if(temp > 255) {RGB_pix[i] = imch(255);}
        else if(temp < 0) {RGB_pix[i] = imch(0);}
        else {RGB_pix[i] = static_cast<imch>(temp);}
    }
    string bmpName = "Seymour_Park.bmp";
    Raw::raw2bmp(bmpName, RGB_pix, 960, 540);

    // system("Yxz_Map_dmax100_b0.85_960x540_24bit.raw");
    system(bmpName.c_str());
    return 0;
}
//================================================================