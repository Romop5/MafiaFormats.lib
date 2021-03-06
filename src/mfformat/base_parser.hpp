// MafiaHub - https://github.com/MafiaHub //

/** 
* @file mfformat/base_parser.hpp
* @author Dominik Madarasz <zaklaus@outlook.com>	
* @author Dominik Madarász <zaklaus@outlook.com>	
* @author Miloslav Číž <tastyfish@seznam.com>	
**/
#ifndef FORMAT_PARSERS_H
#define FORMAT_PARSERS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <string>
#include <mfformat/utils/logger.hpp>
#include <mfformat/utils/math.hpp>

namespace MFFormat
{

/// Abstract class representing a game data format.

class DataFormat
{
public:
    virtual bool load(std::ifstream &srcFile)=0;
    virtual bool save(std::ofstream &dstFile)    { return false; /* optional */ };
    virtual std::string getErrorStr()            { return "Unknown error";      };

protected:
    template<typename T>
    void read(std::ifstream & stream, T* a, size_t size = sizeof(T))
    {
        stream.read((char*)a, size);
    }

    std::streamsize fileLength(std::ifstream &f);

    uint32_t mErrorCode = 0;
};

}

#endif
