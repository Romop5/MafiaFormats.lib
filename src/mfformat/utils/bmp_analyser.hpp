// MafiaHub - https://github.com/MafiaHub //

/** 
* @file mfformat/utils/bmp_analyser.hpp
* @author Miloslav Číž <tastyfish@seznam.com>	
**/
#ifndef BMP_ANALYSER_H
#define BMP_ANALYSER_H

#include <fstream>

namespace MFFormat
{

class BMPInfo
{
public:
    typedef struct
    {
        unsigned char b;
        unsigned char g;
        unsigned char r;
        unsigned char unused;
    } BMPColor;

    bool load(std::ifstream &f);

    BMPColor mTransparentColor;
};

}

#endif
