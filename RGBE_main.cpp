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
string img_name="seymour_park";

clock_t start;
inline void pri_time(clock_t start, clock_t end){
    cout << "Runing Time : "
     << (double)(end-start) / CLOCKS_PER_SEC
     << " seconds" << endl;
}
//================================================================
int main(int argc, char const *argv[]){
    Rgbe_Map img(img_name+".hdr");
    img.Read_HDR();
    img.Info();
    // 色調映射
    start = clock();
    img.Map(100, 0.85, 2.2);
    pri_time(start, clock());
    img.write();
    // 輸出 BMP
    vector<unsigned char> RGB_pix = img;
    Raw::raw2bmp(img_name+".bmp", RGB_pix, 960, 540);
    system((img_name+".bmp").c_str());
    return 0;
}
//================================================================