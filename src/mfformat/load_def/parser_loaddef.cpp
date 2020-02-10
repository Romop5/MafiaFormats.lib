// MafiaHub - https://github.com/MafiaHub //

/** 
* @file mfformat/load_def/parser_loaddef.cpp
* @author Dominik Madarász <zaklaus@outlook.com>	
**/
#include <mfformat/load_def/parser_loaddef.hpp>

namespace MFFormat
{

bool DataFormatLoadDEF::load(std::ifstream &srcFile)
{
    while (srcFile)
    {
        LoadingScreen loadingScreen = {};
        read(srcFile, &loadingScreen);

        if (srcFile)
            mLoadingScreens.push_back(loadingScreen);
    }

    return true;
}

}
