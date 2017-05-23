/*****************************************************************
Name : 
Date : 2017/05/22
By   : CharlotteHonG
Final: 2017/05/22
*****************************************************************/
#pragma warning(disable : 4996)
#include <iostream>
#include <vector>
#include "RGBE.hpp"
#include "RGBE.cpp"
using namespace std;

constexpr char img_name[]("HDRFile/seymour_park");
constexpr char raw_name[]("seymour_park");
//================================================================
int main(int argc, char const *argv[]){
    Rgbe hdr(img_name);
    hdr.Read_HDR();
    hdr.Info();
    hdr.Write_Raw(raw_name);
    hdr.Gray(raw_name);
    return 0;
}
//================================================================
