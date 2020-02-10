// MafiaHub - https://github.com/MafiaHub //

/** 
* @file mfformat/textdb/parser_textdbdef.cpp
* @author Dominik Madarász <zaklaus@outlook.com>	
**/
#include <mfformat/textdb/parser_textdbdef.hpp>

namespace MFFormat
{

bool DataFormatTextdbDEF::load(std::ifstream &srcFile)
{
    Header header = {};
    read(srcFile, &header);

    for (uint32_t  i = 0; i < header.mNumStrings; i++)
    {
        TextBlock textBlock = {};
        read(srcFile, &textBlock);

        mTextBlocks.push_back(textBlock);
    }

    for (auto textBlock : mTextBlocks)
    {
        srcFile.seekg(textBlock.mTextOffset);

        std::string text;
        std::getline(srcFile, text, '\0' );

        mTextEntries.insert(std::make_pair(textBlock.mTextId, text));
    }

    return true;
}

}
