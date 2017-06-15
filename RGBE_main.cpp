/*****************************************************************
Name : 
Date : 2017/05/22
By   : CharlotteHonG
Final: 2017/05/22
*****************************************************************/
#pragma warning(disable : 4996)
#include <iostream>
#include <vector>
#include <ctime>
#include "Rgbe.hpp"
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
    Rgbe_Map hdr2(img_name);
    hdr2.Read_HDR();
    hdr2.Info();
    // 色調映射
    start = clock();
    hdr2.Map(100, 0.85);
    pri_time(start, clock());
    // system();
    return 0;
}
//================================================================