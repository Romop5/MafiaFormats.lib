// MafiaHub - https://github.com/MafiaHub //

/** 
* @file mfformat/road_bin/parser_roadbin.cpp
* @author Dominik Madarász <zaklaus@outlook.com>	
**/
#include <mfformat/road_bin/parser_roadbin.hpp>

namespace MFFormat
{

bool DataFormatRoadBIN::load(std::ifstream &srcFile)
{
    uint32_t header = 0;
    read(srcFile, &header);

    if (header != 0x2)
    {
        return false;
    }

    uint32_t numCrossroads = 0;
    read(srcFile, &numCrossroads);

    for (uint32_t  i = 0; i < numCrossroads; i++)
    {
        Crossroad crossroad = {};
        read(srcFile, &crossroad);

        mCrossroads.push_back(crossroad);
    }

    uint32_t numWaypoints = 0;
    read(srcFile, &numWaypoints);

    for (uint32_t  i = 0; i < numWaypoints; i++)
    {
        Waypoint waypoint = {};
        read(srcFile, &waypoint);

        mWaypoints.push_back(waypoint);
    }

    return true;
}

}
