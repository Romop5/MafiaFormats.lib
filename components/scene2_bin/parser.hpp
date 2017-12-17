#ifndef FORMAT_PARSERS_SCENE2_BIN_H
#define FORMAT_PARSERS_SCENE2_BIN_H

#include <cstring>

#include <base_parser.hpp>

namespace MFFormat
{

class DataFormatScene2BIN: public DataFormat
{
public:
    typedef enum {
        // top Headers
        Header_MISSION = 0x4c53,
        Header_META = 0x0001,
        Header_UNK_FILE = 0xAFFF,
        Header_UNK_FILE2 = 0x3200,
        Header_FOV = 0x3010,
        Header_VIEW_DISTANCE = 0x3011,
        Header_CLIPPING_PLANES = 0x3211,
        Header_WORLD = 0x4000,
        Header_ENTITIES = 0xAE20,
        Header_INIT = 0xAE50,
        // WORLD subHeader
        Header_OBJECT = 0x4010
    } HeaderType;

    typedef enum {
        OBJECT_TYPE = 0x4011,
        OBJECT_POSITION = 0x0020,
        OBJECT_ROTATION = 0x0022,
        OBJECT_POSITION_2 = 0x002C,
        OBJECT_SCALE = 0x002D,
        OBJECT_PARENT = 0x4020,
        OBJECT_NAME = 0x0010, 
        OBJECT_MODEL = 0x2012,
        OBJECT_LIGHT_TYPE = 0x4041,
        OBJECT_LIGHT_COLOUR = 0x0026,
        OBJECT_LIGHT_POWER = 0x4042,
        OBJECT_LIGHT_UNK_1 = 0x4043,
        OBJECT_LIGHT_RANGE = 0x4044,
        OBJECT_LIGHT_FLAGS = 0x4045,
        OBJECT_LIGHT_SECTOR= 0x4046
    } ObjectProperty;

    typedef enum {
        OBJECT_TYPE_LIGHT = 0x02,
        OBJECT_TYPE_CAMERA = 0x03,
        OBJECT_TYPE_SOUND = 0x04,
        OBJECT_TYPE_MODEL = 0x09,
        OBJECT_TYPE_OCCLUDER = 0x0C,
        OBJECT_TYPE_SECTOR = 0x99,
        OBJECT_TYPE_SCRIPT = 0x9B
    } ObjectType;

    #pragma pack(push, 1)
    typedef struct
    {
        uint16_t mType;
        uint32_t mSize;
    } Header;
    #pragma pack(pop)

    typedef struct _Object
    {
        uint32_t mType;
        Vec3 mPos;
        Quat mRot;
        Vec3 mPos2; // NOTE(zaklaus): Final world transform position?
        Vec3 mScale;
        std::string mName;
        std::string mModelName;
        std::string mParentName;

        // Light properties
        float mLightType;
        Vec3 mLightColour;
        int32_t mLightFlags;
        float mLightPower;
        float mLightUnk0;
        float mLightUnk1;
        float mLightNear;
        float mLightFar;
        char mLightSectors[5000];
    } Object;

    virtual bool load(std::ifstream &srcFile);
    
    size_t  getNumObjects();
    Object* getObject(size_t index);
    Object* getObject(std::string name);
    std::unordered_map<std::string, Object> getObjects();

    float getFov();
    void  setFov(float value);

    float getViewDistance();
    void  setViewDistance(float value);

    Vec2  getClippingPlanes();
    void  setClippingPlanes(Vec2 value);
private:
    void readHeader(std::ifstream &srcFile, Header* Header, uint32_t offset);
    void readObject(std::ifstream &srcFile, Header* Header, Object* object);
    
    std::unordered_map<std::string, Object> mObjects;
    float mFov;
    float mViewDistance;
    Vec2  mClippingPlanes;
};

bool DataFormatScene2BIN::load(std::ifstream &srcFile)
{
    Header new_header = {};
    read(srcFile, &new_header);
    uint32_t position = 6;

    while(position + 6 < new_header.mSize)
    {
        srcFile.seekg(position, srcFile.beg);
        Header current_header = {};
        read(srcFile, &current_header);
        readHeader(srcFile, &current_header, position + 6);
        position += current_header.mSize;
    }

    return true;
}

void DataFormatScene2BIN::readHeader(std::ifstream &srcFile, Header* header, uint32_t offset)
{
    switch(header->mType)
    {
        case Header_WORLD:
        {
            uint32_t position = offset;
            while(position + 6 < offset + header->mSize)
            {
                Header next_header = {};
                srcFile.seekg(position, srcFile.beg);
                read(srcFile, &next_header);
                readHeader(srcFile, &next_header, position + 6);
                position += next_header.mSize;
            }
        }
        break;

        case Header_VIEW_DISTANCE:
        {
            read(srcFile, &mViewDistance);
        } 
        break;

        case Header_CLIPPING_PLANES:
        {
            read(srcFile, &mClippingPlanes);
        }
        break;

        case Header_FOV:
        {
            read(srcFile, &mFov);
        } 
        break;

        case Header_OBJECT:
        {
            uint32_t position = offset;
            Object new_object = {};
            while(position + 6 < offset + header->mSize)
            {
                Header next_header = {};
                srcFile.seekg(position, srcFile.beg);
                read(srcFile, &next_header);
                readObject(srcFile, &next_header, &new_object);
                position += next_header.mSize;
            }

            mObjects.insert(make_pair(new_object.mName, new_object));
        } 
        break;
    }
}

void DataFormatScene2BIN::readObject(std::ifstream &srcFile, Header* header, Object* object)
{
    switch(header->mType)
    {
        case OBJECT_TYPE:
        {
            read(srcFile, &object->mType);
        }
        break;

        case OBJECT_NAME:
        {
            char *name = reinterpret_cast<char*>(malloc(header->mSize + 1));
            read(srcFile, name, header->mSize - 1);
            name[header->mSize] = '\0';

            object->mName = std::string(name);
        }
        break;

        case OBJECT_MODEL:
        {
            char *model_name = reinterpret_cast<char*>(malloc(header->mSize + 1));
            read(srcFile, model_name, header->mSize);
            model_name[strlen(model_name) - 4] = '\0';
            sprintf(model_name, "%s.4ds", model_name);
            model_name[header->mSize] = '\0';

            object->mModelName = std::string(model_name);
        }
        break;

        case OBJECT_POSITION:
        {
            Vec3 new_position = {};
            read(srcFile, &new_position);
            object->mPos = new_position;
        } 
        break;

        case OBJECT_ROTATION:
        {
            Quat new_rotation = {};
            read(srcFile, &new_rotation);
            object->mRot = new_rotation;
        } 
        break;

        case OBJECT_POSITION_2:
        {
            Vec3 new_position = {};
            read(srcFile, &new_position);
            object->mPos2 = new_position;
        } 
        break;

        case OBJECT_SCALE:
        {
            Vec3 new_scale = {};
            read(srcFile, &new_scale);
            object->mScale = new_scale;
        } 
        break;

        case OBJECT_LIGHT_TYPE:
        {
            read(srcFile, &object->mLightType);
        }
        break;
        
        case OBJECT_LIGHT_COLOUR:
        {
            read(srcFile, &object->mLightColour);
        }
        break;
        
        case OBJECT_LIGHT_POWER:
        {
            read(srcFile, &object->mLightPower);
        }
        break;
        
        case OBJECT_LIGHT_RANGE:
        {
            read(srcFile, &object->mLightNear);
            read(srcFile, &object->mLightFar);
        }
        break;
        
        case OBJECT_LIGHT_SECTOR:
        {
            read(srcFile, object->mLightSectors, header->mSize);
        }
        break;
        
        case OBJECT_LIGHT_FLAGS:
        {
            read(srcFile, &object->mLightFlags);
        }
        break;

        case OBJECT_LIGHT_UNK_1:
        {
            read(srcFile, &object->mLightUnk0);
            read(srcFile, &object->mLightUnk1);
        }
        break;

        case OBJECT_PARENT:
        {
            Header parent_header = {};
            read(srcFile, &parent_header);
            Object parent_object = {};
            readObject(srcFile, &parent_header, &parent_object);

            object->mParentName = parent_object.mName;
        }
        break;
    }
}

size_t  DataFormatScene2BIN::getNumObjects()
{
    return mObjects.size();
}

DataFormatScene2BIN::Object* DataFormatScene2BIN::getObject(std::string name)
{
    return &mObjects.at(name);
}

std::unordered_map<std::string, DataFormatScene2BIN::Object> DataFormatScene2BIN::getObjects()
{
    return mObjects;
}

float DataFormatScene2BIN::getFov()
{
    return mFov;
}

void DataFormatScene2BIN::setFov(float value)
{
    mFov = value;
}

float DataFormatScene2BIN::getViewDistance()
{
    return mViewDistance;
}

void DataFormatScene2BIN::setViewDistance(float value)
{
    mViewDistance = value;
}

DataFormat::Vec2 DataFormatScene2BIN::getClippingPlanes()
{
    return mClippingPlanes;
}

void DataFormatScene2BIN::setClippingPlanes(DataFormat::Vec2 value)
{
    mClippingPlanes = value;
}

}

#endif