#include <utils.hpp>
#include <fstream>

namespace MFUtil
{

std::string strToLower(std::string str)
{
    std::string result = str;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

std::string strToUpper(std::string str)
{
    std::string result = str;
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

std::string doubleToStr(double value, unsigned int precision)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    return stream.str();
}

bool strStartsWith(std::string str, std::string prefix)
{
    return str.compare(0,prefix.size(),prefix) == 0;
}

std::vector<std::string> strSplit(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    _split(s, delim, std::back_inserter(elems));
    return elems;
}

std::string strReverse(std::string s)
{
    std::string reversed = std::string();
    reversed.resize(s.length());
    std::reverse_copy(s.begin(), s.end(), reversed.begin());
    return reversed;
}

size_t peekLength(std::ifstream &file)
{
    size_t len = 0;
    char currentChar;
    auto stateBefore = file.tellg();

    while(true) 
    {
        file.get(currentChar);
        len++;
        if(currentChar == '\0')
        {
            file.seekg(stateBefore, file.beg);
            return len;
        }    
    }
    return 0;
}

}

