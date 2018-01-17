#ifndef FORMAT_PARSERS_TREE_KLZ_H
#define FORMAT_PARSERS_TREE_KLZ_H

#include <base_parser.hpp>
#include <utils.hpp>
#include <cstring>

namespace MFFormat
{

class DataFormatTreeKLZ: public DataFormat
{
public:
    typedef enum 
    {   
        REFERENCE_FACE = 0x00,
        REFERENCE_XTOBB = 0x80,
        REFERENCE_AABB = 0x81,
        REFERENCE_SPHERE = 0x82,
        REFERENCE_OBB = 0x83,
        REFERENCE_CYLINDER = 0x84
    } GridReference;

    typedef struct
    {
        uint32_t mSignature;
        uint32_t mVersion;
        uint32_t mCollisionDataOffset;
        uint32_t mNumLinks;
        uint32_t mUnknown[2];
    } Header;

    typedef struct
    {
        uint32_t mFlags;
        uint32_t mNameLength;
        char* mName;
    } Link;                       // links to 4DS mesh by name

    #pragma pack(push, 1)
    typedef struct
    {
        float mGridMinX;
        float mGridMinY;
        float mGridMaxX;
        float mGridMaxY;
        float mCellWidth;
        float mCellHeight;
        uint32_t mGridWidth;
        uint32_t mGridHeight;
        uint32_t mUnknown0;
        uint32_t mReserved0[2];
        uint32_t mReserved1;
        uint32_t mNumFaces;
        uint32_t mReserved2;
        uint32_t mNumXTOBBs;
        uint32_t mReserved3;
        uint32_t mNumAABBs;
        uint32_t mReserved4;
        uint32_t mNumSpheres;
        uint32_t mReserved5;
        uint32_t mNumOBBs;
        uint32_t mReserved6;
        uint32_t mNumCylinders;
        uint32_t mReserved7;
        uint32_t mNumUnknownType;  // always 0
        uint32_t mUnknown1;
    } DataHeader;
    #pragma pack(pop)

    typedef struct
    {
        unsigned char mMaterial;
        unsigned char mFlags;
        unsigned char mSortInfo;   // only used with FaceCol
        unsigned char mUnknown;    // different values for different collision types of objects
    } Properties;

    typedef struct
    {
        uint16_t mIndex;
        uint16_t mLink;            // index to link table, this will be the same for all three indices of one face
    } FaceVertexIndex;

    #pragma pack(push, 1)
    typedef struct 
    {    
        Properties mProperties;        // NOTE(ASM): Material (8 bit) | Flags (8 bit) | SortInfo (8 bit) | 0 (8 bit)
        FaceVertexIndex mIndices[3];   
        MFMath::Vec3 mNormal;                  // NOTE(ASM): needs to point in opposite direction compared to the mesh face normal (IIRC!), i.e. if the mesh face normal is (1 0 0), the col face normal needs to be (-1 0 0)
        float mDistance;
    } FaceCol;

    typedef struct 
    {    
        uint32_t mProperties;   // NOTE(ASM): Material(8 bit) | Flags (8 bit) | 0 (8 bit) | 0x81 (8 bit)
        uint32_t mLink;         // NOTE(ASM): index into LinkNameOffsetTable
        MFMath::Vec3 mMin;              // first point that defines the box in space
        MFMath::Vec3 mMax;              // second point that defines the box in space
    } AABBCol;                  // axis-aligned bounding box

    typedef struct 
    {    
        uint32_t mProperties;   // NOTE(ASM): Material(8 bit) | Flags (8 bit) | 0 (8 bit) | 0x80 (8 bit)
        uint32_t mLink;
        MFMath::Vec3 mMin;              // precomputed AABB
        MFMath::Vec3 mMax;
        MFMath::Vec3 mExtends[2];       // BB corners to be transformed
        MFMath::Mat4 mTransform;
        MFMath::Mat4 mInverseTransform;
    } XTOBBCol;                 // oriented bounding box, in addition to OBB has an additional precomputed AABB

    typedef struct 
    {    
        int32_t mProperties;    // NOTE(ASM): Material(8 bit) | Flags (8 bit) | 0 (8 bit) | 0x84 (8 bit)
        uint32_t mLink;
        MFMath::Vec2 mPosition;         // NOTE(ASM): cylinders only have a 2d position!
        float mRadius;
    } CylinderCol;              // cylindrical collision object
  
    typedef struct 
    {    
        uint32_t mProperties;   // NOTE(ASM): Material(8 bit) | Flags (8 bit) | 0 (8 bit) | 0x83 (8 bit)
        uint32_t mLink;
        MFMath::Vec3 mExtends[2];       // two box corners, however the box seems to be symmetrical around [0,0,0], so one is redundant
        MFMath::Mat4 mTransform;
        MFMath::Mat4 mInverseTransform;
    } OBBCol;                   // oriented bounding box

    typedef struct 
    {    
        uint32_t mProperties;   // NOTE(ASM): Material(8 bit) | Flags (8 bit) | 0 (8 bit) | 0x82 (8 bit)
        uint32_t mLink;
        MFMath::Vec3 mPosition;
        float mRadius;
    } SphereCol;                // spherical collision object

    #pragma pack(pop)

    typedef struct 
    {
        uint32_t mNumObjects;
        uint32_t mReserved[2];
        float mHeight;
        uint32_t* mReferences;  // NOTE(ASM): (Type (8 bit)) | (Offset into array of Type (24 bit)))
        uint8_t* mFlags;
    } Cell;                     // grid cell, indexes collision objects in space

    virtual bool load(std::ifstream &srcFile) override;
    std::vector<FaceCol> getFaceCols()                   { return mFaceCols; }
    std::vector<AABBCol> getAABBCols()                   { return mAABBCols; }
    std::vector<XTOBBCol> getXTOBBCols()                 { return mXTOBBCols; }
    std::vector<CylinderCol> getCylinderCols()           { return mCylinderCols; }
    std::vector<OBBCol> getOBBCols()                     { return mOBBCols; }
    std::vector<SphereCol> getSphereCols()               { return mSphereCols; }
    std::vector<Link> getLinks()                         { return mLinkTables; }
    std::vector<std::string> getLinkStrings();
    Cell *getGridCells()                                 { return mGridCellsMemory; }
    Cell getGridCell(unsigned int x, unsigned int y)     { return mGridCellsMemory[y * mDataHeader.mGridWidth + x]; }
    unsigned int getGridWidth()                          { return mDataHeader.mGridWidth; }
    unsigned int getGridHeight()                         { return mDataHeader.mGridHeight; }

    ~DataFormatTreeKLZ();

protected:
    Header mHeader;
    uint32_t* mLinkNameOffsetTable;
    std::vector<Link> mLinkTables;
    float* mCellBoundariesX;
    float* mCellBoundariesY;
    DataHeader mDataHeader;
    uint32_t mCollisionDataMagic;
    std::vector<FaceCol> mFaceCols;
    std::vector<AABBCol> mAABBCols;
    std::vector<XTOBBCol> mXTOBBCols;
    std::vector<CylinderCol> mCylinderCols;
    std::vector<OBBCol> mOBBCols;
    std::vector<SphereCol> mSphereCols;
    uint32_t mCollisionGridMagic;
    Cell* mGridCellsMemory;
};

DataFormatTreeKLZ::~DataFormatTreeKLZ()
{
    for (unsigned int i = 0; i < mDataHeader.mGridWidth * mDataHeader.mGridWidth; ++i)
    {
        if (mGridCellsMemory[i].mNumObjects)
        {
            free(mGridCellsMemory[i].mReferences);
            free(mGridCellsMemory[i].mFlags);
        }
    }

    free(mGridCellsMemory);

    for (unsigned int i = 0; i < mLinkTables.size(); ++i)
        free(mLinkTables[i].mName);

    free(mLinkNameOffsetTable);
    free(mCellBoundariesX); 
    free(mCellBoundariesY); 
}

bool DataFormatTreeKLZ::load(std::ifstream &srcFile)
{
    read(srcFile, &mHeader);
    
    mLinkNameOffsetTable = reinterpret_cast<uint32_t*>(malloc(sizeof(uint32_t)*mHeader.mNumLinks));
    read(srcFile, mLinkNameOffsetTable, sizeof(uint32_t)*mHeader.mNumLinks);

    for (unsigned int i = 0; i < mHeader.mNumLinks; i++) 
    {
        Link newLink = {};
        srcFile.seekg(mLinkNameOffsetTable[i], srcFile.beg);
        read(srcFile, &newLink.mFlags);
      
        newLink.mNameLength = MFUtil::peekLength(srcFile);
        newLink.mName = reinterpret_cast<char*>(malloc(newLink.mNameLength));
        read(srcFile, newLink.mName, newLink.mNameLength);
        mLinkTables.push_back(newLink);
    }

    srcFile.seekg(mHeader.mCollisionDataOffset, srcFile.beg);
    read(srcFile, &mDataHeader);

    mCellBoundariesX = reinterpret_cast<float*>(malloc(sizeof(float)*(mDataHeader.mGridWidth + 1)));
    read(srcFile, mCellBoundariesX, sizeof(float)* (mDataHeader.mGridWidth + 1));
   
    mCellBoundariesY = reinterpret_cast<float*>(malloc(sizeof(float)*(mDataHeader.mGridHeight + 1)));
    read(srcFile, mCellBoundariesY, sizeof(float)* (mDataHeader.mGridHeight + 1));
    read(srcFile, &mCollisionDataMagic);
  
    for (unsigned int i = 0; i < mDataHeader.mNumFaces; i++)
    {
        FaceCol newCol = {};
        read(srcFile, &newCol);
        mFaceCols.push_back(newCol);
    }

    for (unsigned int i = 0; i < mDataHeader.mNumAABBs; i++)
    {
        AABBCol newCol = {};
        read(srcFile, &newCol);
        mAABBCols.push_back(newCol);
    }

    for (unsigned int i = 0; i < mDataHeader.mNumXTOBBs; i++)
    {
        XTOBBCol newCol = {};
        read(srcFile, &newCol);
        mXTOBBCols.push_back(newCol);
    }

    for (unsigned int i = 0; i < mDataHeader.mNumCylinders; i++)
    {
        CylinderCol newCol = {};
        read(srcFile, &newCol);
        mCylinderCols.push_back(newCol);
    }

    for (unsigned int i = 0; i < mDataHeader.mNumOBBs; i++)
    {
        OBBCol newCol = {};
        read(srcFile, &newCol);
        mOBBCols.push_back(newCol);
    }

    for (unsigned int i = 0; i < mDataHeader.mNumSpheres; i++)
    {
        SphereCol newCol = {};
        read(srcFile, &newCol);
        mSphereCols.push_back(newCol);
    }
   
    read(srcFile, &mCollisionGridMagic);
    uint32_t gridSize = mDataHeader.mGridWidth * mDataHeader.mGridWidth;
    mGridCellsMemory = reinterpret_cast<Cell*>(malloc(sizeof(Cell) * gridSize));

    for (unsigned int i = 0; i < gridSize; i++)
    {
        read(srcFile, &mGridCellsMemory[i].mNumObjects);
        read(srcFile, mGridCellsMemory[i].mReserved, sizeof(uint32_t)*2);
        read(srcFile, &mGridCellsMemory[i].mHeight);

        if (mGridCellsMemory[i].mNumObjects)
        {
            mGridCellsMemory[i].mReferences = reinterpret_cast<uint32_t*>(malloc(sizeof(uint32_t) * mGridCellsMemory[i].mNumObjects));
            read(srcFile, mGridCellsMemory[i].mReferences, sizeof(uint32_t) * mGridCellsMemory[i].mNumObjects);

            // NOTE(ASM): needs to be aligned to 4 bytes, purpose unknown
            mGridCellsMemory[i].mFlags = reinterpret_cast<uint8_t*>(malloc((mGridCellsMemory[i].mNumObjects + 3) /4 * sizeof(uint32_t)));
            read(srcFile, mGridCellsMemory[i].mFlags, (mGridCellsMemory[i].mNumObjects + 3) /4 * sizeof(uint32_t));
        }
    }

    return true;
}

std::vector<std::string> DataFormatTreeKLZ::getLinkStrings()
{
    std::vector<std::string> result;

    for (int i = 0; i < (int) mLinkTables.size(); ++i)
    {
        char buffer[255];
        std::memcpy(buffer,mLinkTables[i].mName, mLinkTables[i].mNameLength);
        buffer[mLinkTables[i].mNameLength] = 0;
        result.push_back(buffer);
    }

    return result;
}

}

#endif
