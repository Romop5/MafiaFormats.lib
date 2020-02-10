// MafiaHub - https://github.com/MafiaHub //

/** 
* @file mfformat/load_def/parser_loaddef.hpp
* @author Dominik Madarász <zaklaus@outlook.com>	
**/
#ifndef FORMAT_PARSERS_LOAD_DEF_H
#define FORMAT_PARSERS_LOAD_DEF_H

#include <mfformat/base_parser.hpp>

namespace MFFormat
{

class DataFormatLoadDEF: public DataFormat
{
public:
    #pragma pack(push,1)
    typedef struct
    {
        char mMissionName[32];
        char mFileName[32];
        uint32_t mTextId;
    } LoadingScreen;
    #pragma pack(pop)

    virtual bool load(std::ifstream &srcFile) override;
    inline std::vector<LoadingScreen> getLoadingScreens()   { return mLoadingScreens; }
    inline size_t getNumLoadingScreens()                    { return mLoadingScreens.size(); }

private:
    std::vector<LoadingScreen> mLoadingScreens;
};

}

#endif
