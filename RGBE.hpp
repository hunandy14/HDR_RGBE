/*****************************************************************
Name :
Date : 2017/05/22
By   : CharlotteHonG
Final: 2017/05/22

source: http://www.graphics.cornell.edu/~bjw/rgbe/
*****************************************************************/
#pragma warning(disable : 4996)
#pragma once
using namespace std;
//----------------------------------------------------------------
#ifndef RGB_t_byCHG
#define RGB_t_byCHG
// RGB –ÕÑe
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
// È_ôn¿˝Õ‚
class bad_openFile : public std::runtime_error {
public:
    bad_openFile(const std::string& str): std::runtime_error(str) {}
};
#endif // RGB_t_byCHG
//----------------------------------------------------------------
using imch = unsigned char;
class Rgbe {
public:
    Rgbe(string name);
    virtual ~Rgbe() = default;
    virtual operator vector<float>&() { return HDR_pix; }
    virtual operator vector<imch>() { return toRaw(HDR_pix); }
public:
    static float& r3dim(vector<float>& pix, size_t idx, RGB_t rgb);
public:
    float& at_HDR(size_t idx, RGB_t rgb);
    const float& at_HDR(size_t idx, RGB_t rgb) const;
    const float& r2d3dim(vector<float>& pix,
                         size_t y, size_t x, RGB_t rgb) const;
    float& r2d3dim(vector<float>& pix,
                   size_t y, size_t x, RGB_t rgb);
public:
    static void rgb2xyz(vector<float>& XYZ_pix, vector<float>& RGB_pix);
    static void xyz2Yxy(vector<float>& Yxy_pix, vector<float>& XYZ_pix);
    static void rgb2Yxy(vector<float>& Yxy_pix, vector<float>& RGB_pix);
    static void rgb2Yxy(vector<float>& RGB_pix);
    static void Yxz2xyz(vector<float>& XYZ_pix, vector<float>& Yxy_pix);
    static void xyz2rgb(vector<float>& RGB_pix, vector<float>& XYZ_pix);
    static void Yxz2rgb(vector<float>& RGB_pix, vector<float>& Yxy_pix);
    static void Yxz2rgb(vector<float>& Yxy_pix);
    static auto toRaw(vector<float>& pix)-> vector<imch>;
    static void Write_raw(vector<float>& pix, string name);
public:
    void Read_HDR();
public:
    virtual void Info();
    virtual string Out_name(string name, string ref);
protected:
    string file_name;
    int img_width = 0;
    int img_height = 0;
protected:
    vector<float> HDR_pix;
};
//----------------------------------------------------------------
class Rgbe_Map: public Rgbe {
public:
    Rgbe_Map(string name): Rgbe(name) {}
    ~Rgbe_Map() {}
    float& at_Map(size_t idx, RGB_t rgb);
    const float& at_Map(size_t idx, RGB_t rgb) const;
    operator vector<float>&() {return Map_pix;}
    operator vector<imch>() { return toRaw(Map_pix); }
public:
    static void Mapping(vector<float>& lumi,
                        float dmax=100, float b=0.85);
    static void gama_fix(vector<float>& RGB_pix, float gam);
public:
    void Map(float dmax=100, float b=0.85, float gam=2.2);
    string Out_name(string name, string ref);
    void write();
public:
    float para[3];
    vector<float> Map_pix;
};
//----------------------------------------------------------------
// Inline function
inline float& Rgbe::r3dim(vector<float>& pix, size_t idx, RGB_t rgb) {
    return pix[(idx*3)+rgb];
}

inline float& Rgbe::at_HDR(size_t idx, RGB_t rgb) {
    return const_cast<float&>(
               static_cast<const Rgbe&>(*this).at_HDR(idx, (rgb)));
}
inline const float& Rgbe::at_HDR(size_t idx, RGB_t rgb) const {
    return HDR_pix[(idx*3)+rgb];
}

inline const float& Rgbe::r2d3dim(vector<float>& pix,
                                  size_t y, size_t x, RGB_t rgb) const
{
    size_t idx = y*img_width + x;
    return pix[(idx*3)+rgb];
}
inline float& Rgbe::r2d3dim(vector<float>& pix,
                            size_t y, size_t x, RGB_t rgb)
{
    return const_cast<float&>(
               static_cast<const Rgbe&>(*this).r2d3dim(pix, y, x, rgb));
}
//----------------------------------------------------------------
inline float& Rgbe_Map::at_Map(size_t idx, RGB_t rgb) {
    return const_cast<float&>(
               static_cast<const Rgbe_Map&>(*this).at_Map(idx, (rgb)));
}
inline const float& Rgbe_Map::at_Map(size_t idx, RGB_t rgb) const {
    return Map_pix[(idx*3)+rgb];
}
//----------------------------------------------------------------
