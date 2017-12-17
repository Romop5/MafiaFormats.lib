#ifndef FORMAT_PARSERS_H
#define FORMAT_PARSERS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <string>
#include <logger_console.hpp>

namespace MFFormat
{

/// Abstract class representing a game data format.

class DataFormat
{
public:
    virtual bool load(std::ifstream &srcFile)=0;
    virtual bool save(std::ofstream &dstFile) { return false; /* optional */ };

    #pragma pack(push, 1)
    typedef struct
    {
        float x;
        float y;
        float z;

        inline std::string str()
        {
            std::stringstream sstream;

            sstream << x << ", " << y << ", " << z;

            return sstream.str();
        }
    } Vec3;

    typedef struct
    {
        float x;
        float y;

        inline std::string str()
        {
            std::stringstream sstream;

            sstream << x << ", " << y;

            return sstream.str();
        }
    } Vec2;

    typedef struct
    {
        float x;
        float y;
        float z;
        float w;

        inline std::string str()
        {
            std::stringstream sstream;

            sstream << x << ", " << y << ", " << z << ", " << w;

            return sstream.str();
        }
    } Quat;

    typedef struct
    {
        float a0, a1, a2, a3;
        float b0, b1, b2, b3;
        float c0, c1, c2, c3;
        float d0, d1, d2, d3;
    } Mat4;

    #pragma pack(pop)
protected:
    template<typename T>
    void read(std::ifstream & stream, T* a, size_t size = sizeof(T))
    {
        stream.read((char*)a, size);
    }

    std::streamsize fileLength(std::ifstream &f);
};

}

#endif
