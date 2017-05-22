/*****************************************************************
Name : 
Date : 2017/05/22
By   : CharlotteHonG
Final: 2017/05/22

source: http://www.graphics.cornell.edu/~bjw/rgbe/
*****************************************************************/
#include <iostream>
#include <vector>
#include "rgbe\rgbe.h"
#include "rgbe\rgbe.c"
using namespace std;

class RGBE{
public:
    RGBE(string name): file_name(name){
        FILE* HDR_File;
        file_name += ".hdr";
        if (!(HDR_File = fopen(file_name.c_str(), "rb"))){
            cout << "\n Can't open the img !!" << endl;
            exit(0);
        } fclose(HDR_File);
    }
    void Read_HDR(){
        FILE* HDR_File;
        HDR_File = fopen(file_name.c_str(), "rb");
        RGBE_ReadHeader(HDR_File, &img_width, &img_height, NULL);
        Info();
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

        FILE* HDR_File;
        if (!(HDR_File = fopen(name.c_str(), "wb"))){
            cout << "\n Can't open the raw !!" << endl;
            system("pause");
            exit(0);
        }

        // char *Raw_pix;
        // Raw_pix = (char*)malloc(sizeof(char)* 3 * image_height * image_width);
        size_t len = Canvas_Size()*3;
        Raw_pix.resize(len);
        for(unsigned i = 0; i < len; ++i) {
            float temp = round(HDR_pix[i]*255);
            if(temp > 255) {
                Raw_pix[i] = 255;
            } else if(temp < 0){
                Raw_pix[i] = 0;
            } else{
                Raw_pix[i] = temp;
            }
        }
        fwrite (Raw_pix.data() , sizeof(char), len, HDR_File);
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
    string file_name;
    int img_width = 0;
    int img_height = 0;
    vector<float> HDR_pix;
    vector<char> Raw_pix;
};