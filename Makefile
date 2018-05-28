all: RGBE_main.exe

CXX := g++
CFLAGS := -O3 -Wall -std=c++11

RGBE_main.exe: rgbe_lib.o Raw2img.o RGBE.o Rgb_Mapping.o
	$(CXX) $(CFLAGS) *o -o RGBE_main.exe

rgbe_lib.o: RGBE/rgbe_lib.cpp RGBE/rgbe_lib.hpp
	$(CXX) $(CFLAGS) -c RGBE/rgbe_lib.cpp
Raw2img.o: Raw2Img/Raw2img.cpp Raw2Img/Raw2img.hpp
	$(CXX) $(CFLAGS) -c Raw2Img/Raw2img.cpp
RGBE.o: RGBE.cpp RGBE.hpp
	$(CXX) $(CFLAGS) -c RGBE.cpp
Rgb_Mapping.o: Rgb_Mapping.cpp
	$(CXX) $(CFLAGS) -c Rgb_Mapping

