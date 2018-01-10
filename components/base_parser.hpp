#ifndef FORMAT_PARSERS_H
#define FORMAT_PARSERS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <string>
#include <loggers/console.hpp>

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

        /// Converts the quaternion from Mafia format.

        void fromMafia()
        {
            float x0 = x;
            float y0 = y;
            float z0 = z;
            float w0 = w;

            x = y0;
            y = z0;
            z = w0;
            w = -1 * x0;
        }
    } Quat;

    typedef struct
    {
        float a0, a1, a2, a3;
        float b0, b1, b2, b3;
        float c0, c1, c2, c3;
        float d0, d1, d2, d3;

        inline std::string str()
        {
            std::stringstream sstream;

            sstream << a0 << " " << a1 << " " << a2 << " " << a3 << std::endl;
            sstream << b0 << " " << b1 << " " << b2 << " " << b3 << std::endl;
            sstream << c0 << " " << c1 << " " << c2 << " " << c3 << std::endl;
            sstream << d0 << " " << d1 << " " << d2 << " " << d3 << std::endl;
            
            return sstream.str();
        }

        Vec3 getTranslation()
        {
            Vec3 result;
            result.x = d0;
            result.y = d1;
            result.z = d2;
            return result;
        }

        Vec3 getScale()
        {
            Vec3 result;
            result.x = sqrt(a0 * a0 + a1 * a1 + a2 * a2);
            result.y = sqrt(b0 * b0 + b1 * b1 + b2 * b2);
            result.z = sqrt(c0 * c0 + c1 * c1 + c2 * c2);
            return result;
        }

    } Mat4;
    #pragma pack(pop)

    virtual std::string getErrorStr() { return "Unknown error"; };
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
