// MafiaHub - https://github.com/MafiaHub //

/** 
* @file mfformat/effects/parser_effects_bin.cpp
* @author RoadTrain <pashtet92@bk.ru>	
**/
#include <mfformat/effects/parser_effects_bin.hpp>

namespace MFFormat
{

bool DataFormatEffectsBIN::load(std::ifstream &srcFile)
{
    Header header = {};
    read(srcFile, &header);

    if (header.mMagic != 0x64)
    {
        return false;
    }

    while (srcFile)
    {
        Effect effect = {};
        read(srcFile, &effect);

        if (srcFile)
            mEffects.push_back(effect);
    }

    return true;
}

}
