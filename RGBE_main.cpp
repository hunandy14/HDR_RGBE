/*****************************************************************
Name : 
Date : 2017/05/22
By   : CharlotteHonG
Final: 2017/05/22
*****************************************************************/
#pragma warning(disable : 4996)
#include <iostream>
#include <vector>
#include "Rgbe.hpp"
#include "Single_file.hpp"
using namespace std;

constexpr char img_name[]("HDRFile/seymour_park");
constexpr char raw_name[]("seymour_park");
//================================================================
int main(int argc, char const *argv[]){
    Rgbe2Raw hdr(img_name);
    hdr.Read_HDR();
    hdr.Info();
    hdr.Write_Raw(raw_name);
    // hdr.Write_Gray(raw_name);
    // cout << (float)hdr.at_HDR(0, R) << endl;
    return 0;
}
//================================================================
