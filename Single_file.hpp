/*****************************************************************
Name : GCC單檔編譯
Date : 2017/05/23
By   : CharlotteHonG
Final: 2017/05/23
*****************************************************************/
#pragma once


#if defined(_MSC_VER)
#endif


#if defined(__GNUC__)
   #include "RGBE\rgbe_lib.cpp"
   #include "Rgbe.hpp"
   #include "Rgbe.cpp"
   #include "Rgbe2Raw.cpp"
   #include "Rgb_Mapping.cpp"
#endif