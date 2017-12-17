#ifndef FORMAT_PARSERS_4DS_H
#define FORMAT_PARSERS_4DS_H

#include <base_parser.hpp>

namespace MFFormat
{

class DataFormat4DS: public DataFormat
{
public:
    typedef enum
    {
        MATERIALFLAG_TEXTUREDIFFUSE = 0x00040000,
        MATERIALFLAG_COLORED = 0x08000000,
        MATERIALFLAG_MIPMAPPING = 0x00800000,
        MATERIALFLAG_ANIMATEDTEXTUREDIFFUSE = 0x04000000,
        MATERIALFLAG_ANIMATEXTEXTUREALPHA = 0x02000000,
        MATERIALFLAG_DOUBLESIDEDMATERIAL = 0x10000000,
        MATERIALFLAG_ENVIRONMENTMAP = 0x00080000,
        MATERIALFLAG_NORMALTEXTUREBLEND = 0x00000100,
        MATERIALFLAG_MULTIPLYTEXTUREBLEND = 0x00000200,
        MATERIALFLAG_ADDITIVETEXTUREBLEND = 0x00000400,
        MATERIALFLAG_CALCREFLECTTEXTUREY = 0x00001000,
        MATERIALFLAG_PROJECTREFLECTTEXTUREY = 0x00002000,
        MATERIALFLAG_PROJECTREFLECTTEXTUREZ = 0x00004000,
        MATERIALFLAG_ADDITIONALEFFECT = 0x00008000,
        MATERIALFLAG_ALPHATEXTURE = 0x40000000,
        MATERIALFLAG_COLORKEY = 0x20000000,
        MATERIALFLAG_ADDITIVEMIXING = 0x80000000
    } MaterialFlag;

    typedef enum
    {
        MESHTYPE_STANDARD = 0x01,
        MESHTYPE_SECTOR = 0x05,
        MESHTYPE_DUMMY = 0x06,
        MESHTYPE_TARGET = 0x07,
        MESHTYPE_BONE = 0x0a
    } MeshType;

    typedef enum
    {
        VISUALMESHTYPE_STANDARD = 0x0,
        VISUALMESHTYPE_SINGLEMESH = 0x02,
        VISUALMESHTYPE_SINGLEMORPH = 0x03,
        VISUALMESHTYPE_BILLBOARD = 0x04,
        VISUALMESHTYPE_MORPH = 0x05,
        VISUALMESHTYPE_GLOW = 0x06,
        VISUALMESHTYPE_MIRROR = 0x08
    } VisualMeshType;

    typedef enum
    {
        MESHRENDERFLAG_USEDEPTHBIAS = 0x0001,
        MESHRENDERFLAG_USESHADOWS = 0x0002,
        MESHRENDERFLAG_UNKNOWN = 0x0008,
        MESHRENDERFLAG_USEPROJECTION = 0x0020,
        MESHRENDERFLAG_FORBIDFOG = 0x0080
    } MeshRenderFlag;

    typedef enum
    {
        MESHOCCLUDINGFLAG_NORMAL = 0x09,
        MESHOCCLUDINGFLAG_SECTOR = 0x7D,
        MESHOCCLUDINGFLAG_WALL = 0x3D,     // mesh in sector (walls)
        MESHOCCLUDINGFLAG_PORTAL = 0x1D,   // mesh in portal
        MESHOCCLUDINGFLAG_INACTIVE = 0x11
    } MeshOccludingFlag;

    typedef struct
    {
        uint32_t mFlags;
        Vec3 mAmbient;
        Vec3 mDiffuse;
        Vec3 mEmission;
        float mTransparency; // 0.0 - invisible; 1.0 - opaque

        // environment map
        float mEnvRatio;
        char mEnvMapNameLength;
        char mEnvMapName[255];

        char mDiffuseMapNameLength;
        char mDiffuseMapName[255];

        // alpha map
        char mAlphaMapNameLength;
        char mAlphaMapName[255];

        // anim map
        uint32_t mAnimSequenceLength;
        uint16_t mUnk0;
        uint32_t mFramePeriod;
        uint32_t mUnk1;
        uint32_t mUnk2;
    } Material;

    typedef struct
    {
        Vec3 mPos;
        Vec3 mNormal;
        Vec2 mUV;
    } Vertex;

    typedef struct
    {
        uint16_t mA;
        uint16_t mB;
        uint16_t mC;
    } Face;

    typedef struct
    {
        uint16_t mFaceCount;
        Face *mFaces;
        uint16_t mMaterialID;
    } FaceGroup;

    typedef struct
    {
        float mRelativeDistance;
        uint16_t mVertexCount;
        Vertex *mVertices;
        uint8_t mFaceGroupCount;
        FaceGroup *mFaceGroups;
    } Lod;

    typedef struct
    {
        uint16_t mInstanced;
        // for non-instanced mesh
        uint8_t mLODLevel;
        Lod *mLODs;
    } Standard;

    typedef struct
    {
        uint16_t mUnk0;
        uint8_t mTargetCount;
        uint16_t *mTargets;
    } Target;

    typedef struct
    {
        float mTransform[16];
        uint32_t mBoneID;
    } Bone;

    typedef struct
    {
        uint8_t mVertexCount;
        uint32_t mUnk0; // always 4.
        uint32_t mUnk1[6];
        Vec3 *mVertices;
    } Portal;

    typedef struct
    {
        uint32_t mUnk0; // always 2049.
        uint32_t mUnk1; // always 0.
        uint32_t mVertexCount;
        uint32_t mFaceCount;
        Vec3 *mVertices;
        Face *mFaces;
        Vec3 mMinBox;
        Vec3 mMaxBox;
        uint8_t mPortalCount;
        Portal *mPortals;
    } Sector;

    typedef struct
    {
        Standard mStandard;
        uint32_t mRotationAxis;
        uint8_t mIgnoreCamera;
    } Billboard;

    typedef struct
    {
        // bounding box
        Vec3 mMinBox;
        Vec3 mMaxBox;
    } Dummy;

    typedef struct
    {
        float mPosition;
        uint16_t mMaterialID;
    } GlowData;

    typedef struct
    {
        uint8_t mGlowCount;
        GlowData *mGlowData;
    } Glow;

    typedef struct
    {
        Vec3 mMinBox;
        Vec3 mMaxBox;
        float mUnk[4];
        Mat4 mReflectionMatrix;
        Vec3 mBackgroundColor;
        float mViewDistance;
        uint32_t mVertexCount;
        uint32_t mFaceCount;
        Vec3 *mVertices;
        Face *mFaces;
    } Mirror;

    typedef struct
    {
        Vec3 mPosition;
        Vec3 mNormals;
    } MorphLodVertex;

    typedef struct
    {
        uint16_t mVertexCount;
        MorphLodVertex *mVertices;
        uint8_t mUnk0;
        uint16_t *mVertexLinks; // addresses vertices from Standard's LOD mesh  
    } MorphLod;

    typedef struct
    {
        Standard mStandard;
        uint8_t mFrameCount;
        uint8_t mLODLevel;      // should be equal to Standard.LODLevel
        uint8_t mUnk0;
        MorphLod *mLODs;
        Vec3 mMinBox;
        Vec3 mMaxBox;
        float mUnk1[4];
    } Morph;

    typedef struct
    {
        float mTransform[16];
        uint32_t mUnk0;
        uint32_t mAdditionalValuesCount;
        uint32_t mBoneID;
        Vec3 mMinBox;
        Vec3 mMaxBox;
        float *mAdditionalValues;
    } SingleMeshLodJoint;

    typedef struct
    {
        uint8_t mJointCount;
        uint32_t mUnk0;
        Vec3 mMinBox;
        Vec3 mMaxBox;
        SingleMeshLodJoint *mJoints;
    } SingleMeshLod;

    typedef struct
    {
        Standard mStandard;
        SingleMeshLod *mLODs; // LODLevel == Standard.LODLevel.
    } SingleMesh;

    typedef struct
    {
        SingleMesh mSingleMesh;
        Morph mMorph;         // Morph without Standard Mesh!
    } SingleMorph;

    typedef struct
    {
        uint8_t mMeshType;
        // standard mesh type
        uint8_t mVisualMeshType;
        uint16_t mMeshRenderFlags;
        uint16_t mParentID; // 0 - not connected
        Vec3 mPos;
        Vec3 mScale;
        Quat mRot;
        uint8_t mCullingFlags;
        uint8_t mMeshNameLength;
        char mMeshName[255];
        uint8_t mMeshParamsLength;
        char mMeshParams[255];
        Standard mStandard;
        Dummy mDummy;
        Mirror mMirror;
        Glow mGlow;
        Billboard mBillboard;
        Sector mSector;
        Target mTarget;
        Bone mBone;
        Morph mMorph;
        SingleMesh mSingleMesh;
        SingleMorph mSingleMorph;
    } Mesh;

    typedef struct
    {
        uint8_t mSignature[4];
        uint16_t mFormatVersion; // PC : 0x1D (29)
        uint64_t mTimestamp;
        uint16_t mMaterialCount;
        Material *mMaterials;
        uint16_t mMeshCount;
        Mesh *mMeshes;
        uint8_t mUse5DS;
    } Model;

    virtual bool load(std::ifstream &srcFile) override;
    
    inline Model* getModel()
    {
        return mLoadedModel;
    }

protected:
    void loadMaterial(Model *model, std::ifstream &file);
    Lod loadLod(std::ifstream &file);
    Standard loadStandard(std::ifstream &file);
    Mirror loadMirror(std::ifstream &file);
    Glow loadGlow(std::ifstream &file);
    Portal loadPortal(std::ifstream &file);
    Sector loadSector(std::ifstream &file);
    Target loadTarget(std::ifstream &file);
    Morph loadMorph(std::ifstream &file, bool ignoreStandard);
    SingleMeshLodJoint loadSingleMeshLodJoint(std::ifstream &file);
    SingleMeshLod loadSingleMeshLod(std::ifstream &file);
    SingleMesh loadSingleMesh(std::ifstream &file);
    SingleMorph loadSingleMorph(std::ifstream &file);
    void loadMesh(Model *model, std::ifstream &file);
    Model* loadModel(std::ifstream &file);
    Model* mLoadedModel;
};

void DataFormat4DS::loadMaterial(Model *model, std::ifstream &file)
{
    read(file, &model->mMaterialCount);
    model->mMaterials = reinterpret_cast<Material*>(malloc(sizeof(Material) * model->mMaterialCount));
    
    for(size_t i = 0; i < model->mMaterialCount; ++i) 
    {
        Material mat = {};
        read(file, &mat.mFlags);
        read(file, &mat.mAmbient);
        read(file, &mat.mDiffuse);
        read(file, &mat.mEmission);
        read(file, &mat.mTransparency);

        if(mat.mFlags & MATERIALFLAG_ENVIRONMENTMAP) 
        {
            read(file, &mat.mEnvRatio);
            read(file, &mat.mEnvMapNameLength);
            read(file, &mat.mEnvMapName, mat.mEnvMapNameLength);
        }

        read(file, &mat.mDiffuseMapNameLength);
        read(file, mat.mDiffuseMapName, mat.mDiffuseMapNameLength);
       
            
        if(mat.mFlags & MATERIALFLAG_ALPHATEXTURE)
        {
            read(file, &mat.mAlphaMapNameLength);
            read(file, mat.mAlphaMapName, mat.mAlphaMapNameLength);
        }
        
        if(mat.mFlags & MATERIALFLAG_ANIMATEDTEXTUREDIFFUSE)
        {
            read(file, &mat.mAnimSequenceLength);
            read(file, &mat.mUnk0);
            read(file, &mat.mFramePeriod);
            read(file, &mat.mUnk1);
            read(file, &mat.mUnk2);
        }

        model->mMaterials[i] = mat;
    }  
}

DataFormat4DS::Lod DataFormat4DS::loadLod(std::ifstream &file)
{
    Lod new_lod = {};
    read(file, &new_lod.mRelativeDistance);
    read(file, &new_lod.mVertexCount);

    new_lod.mVertices = reinterpret_cast<Vertex*>(malloc(sizeof(Vertex)* new_lod.mVertexCount));
    read(file, new_lod.mVertices, sizeof(Vertex)*new_lod.mVertexCount);
    read(file, &new_lod.mFaceGroupCount); 

    new_lod.mFaceGroups = reinterpret_cast<FaceGroup*>(malloc(sizeof(FaceGroup)*new_lod.mFaceGroupCount));
    
    for(size_t i = 0; i < new_lod.mFaceGroupCount; ++i)
    {
        FaceGroup new_face_group = {};
        read(file, &new_face_group.mFaceCount);
        new_face_group.mFaces = reinterpret_cast<Face*>(malloc(sizeof(Face)*new_face_group.mFaceCount));
        read(file, new_face_group.mFaces, sizeof(Face)*new_face_group.mFaceCount);
        read(file, &new_face_group.mMaterialID);

        new_lod.mFaceGroups[i] = new_face_group;
    }

    return new_lod;
}

DataFormat4DS::Standard DataFormat4DS::loadStandard(std::ifstream &file)
{
    Standard new_standard = {};
    read(file, &new_standard.mInstanced);

    if(!new_standard.mInstanced)
    {
        read(file, &new_standard.mLODLevel);
        new_standard.mLODs = reinterpret_cast<Lod*>(malloc(sizeof(Lod)*new_standard.mLODLevel));

        for(size_t i = 0; i < new_standard.mLODLevel; ++i)
        {
            Lod new_lod = {};
            new_lod = loadLod(file);
            new_standard.mLODs[i] = new_lod;
        }
    }

    return new_standard;
}

DataFormat4DS::Mirror DataFormat4DS::loadMirror(std::ifstream &file)
{
    Mirror new_mirror = {};
    read(file, &new_mirror.mMinBox);
    read(file, &new_mirror.mMaxBox);
    read(file, &new_mirror.mUnk, sizeof(float) * 4);
    read(file, &new_mirror.mReflectionMatrix);
    read(file, &new_mirror.mBackgroundColor);
    read(file, &new_mirror.mViewDistance);
    read(file, &new_mirror.mVertexCount);
    read(file, &new_mirror.mFaceCount);

    new_mirror.mVertices = reinterpret_cast<Vec3*>(malloc(sizeof(osg::Vec3f)*new_mirror.mVertexCount));
    new_mirror.mFaces = reinterpret_cast<Face*>(malloc(sizeof(Face)*new_mirror.mFaceCount));
    
    read(file, new_mirror.mVertices, sizeof(Vec3)*new_mirror.mVertexCount);
    read(file, new_mirror.mFaces, sizeof(Face)*new_mirror.mFaceCount);
    
    return new_mirror;
}

DataFormat4DS::Glow DataFormat4DS::loadGlow(std::ifstream &file)
{
    Glow new_glow = {};
    read(file, &new_glow.mGlowCount);
    new_glow.mGlowData = reinterpret_cast<GlowData*>(malloc(sizeof(GlowData)*new_glow.mGlowCount));
    
    for(size_t i = 0; i < new_glow.mGlowCount; ++i)
    {
        GlowData new_glow_data = {};
        read(file, &new_glow_data.mPosition);
        read(file, &new_glow_data.mMaterialID);
        
        new_glow.mGlowData[i] = new_glow_data;
    }
    
    return new_glow;
}

DataFormat4DS::Portal DataFormat4DS::loadPortal(std::ifstream &file)
{
    Portal new_portal = {};
    read(file, &new_portal.mVertexCount);
    read(file, &new_portal.mUnk0);
    read(file, new_portal.mUnk1, sizeof(float) * 6);
    new_portal.mVertices = reinterpret_cast<Vec3*>(malloc(sizeof(osg::Vec3f)*new_portal.mVertexCount));
    read(file, new_portal.mVertices, sizeof(Vec3)*new_portal.mVertexCount); 
   
    return new_portal;
}

DataFormat4DS::Sector DataFormat4DS::loadSector(std::ifstream &file)
{
    Sector new_sector = {};
    read(file, &new_sector.mUnk0);
    read(file, &new_sector.mUnk1);
    read(file, &new_sector.mVertexCount);
    read(file, &new_sector.mFaceCount);

    new_sector.mVertices = reinterpret_cast<Vec3*>(malloc(sizeof(Vec3)*new_sector.mVertexCount));
    read(file, new_sector.mVertices, sizeof(Vec3)*new_sector.mVertexCount);

    new_sector.mFaces = reinterpret_cast<Face*>(malloc(sizeof(Face)*new_sector.mFaceCount));
    read(file, new_sector.mFaces, sizeof(Face) *new_sector.mFaceCount);

    read(file, &new_sector.mMinBox);
    read(file, &new_sector.mMaxBox);
    read(file, &new_sector.mPortalCount);

    new_sector.mPortals = reinterpret_cast<Portal*>(malloc(sizeof(Portal)*new_sector.mPortalCount));

    for(size_t i = 0; i < new_sector.mPortalCount; ++i) 
    {
        Portal new_portal = {};
        new_portal = loadPortal(file);
        new_sector.mPortals[i] = new_portal;
    }

    return new_sector;
}

DataFormat4DS::Target DataFormat4DS::loadTarget(std::ifstream &file)
{
    Target new_target = {};
    read(file, &new_target.mUnk0);
    read(file, &new_target.mTargetCount);
    
    new_target.mTargets = reinterpret_cast<uint16_t*>(malloc(sizeof(uint16_t)*new_target.mTargetCount));
    read(file, new_target.mTargets, sizeof(uint16_t)*new_target.mTargetCount);
    
    return new_target;
}

DataFormat4DS::Morph DataFormat4DS::loadMorph(std::ifstream &file, bool ignoreStandard)
{
    Morph new_morph = { };
    // NOTE(zaklaus): Single Morph contains Standard Mesh in Single Mesh already.
    if(!ignoreStandard) 
    {
        new_morph.mStandard = loadStandard(file);
    } 
    // NOTE(zaklaus): ELSE ignore Standard Mesh, since Single Mesh has it.
    
    read(file, &new_morph.mFrameCount);
    
    if(new_morph.mFrameCount)
    {
        read(file, &new_morph.mLODLevel);
        read(file, &new_morph.mUnk0);

        new_morph.mLODs = reinterpret_cast<MorphLod*>(malloc(sizeof(MorphLod)*new_morph.mLODLevel));

        for(size_t i = 0; i < new_morph.mLODLevel; ++i) 
        {
            MorphLod new_morph_lod = {};
            read(file, &new_morph_lod.mVertexCount);

            new_morph_lod.mVertices = reinterpret_cast<MorphLodVertex*>(malloc(sizeof(MorphLodVertex)*new_morph_lod.mVertexCount * new_morph.mFrameCount));
            read(file, new_morph_lod.mVertices, sizeof(MorphLodVertex)*new_morph_lod.mVertexCount * new_morph.mFrameCount);

            if(new_morph_lod.mVertexCount * new_morph.mFrameCount) 
            {
                read(file, &new_morph_lod.mUnk0);
            }

            new_morph_lod.mVertexLinks = reinterpret_cast<uint16_t*>(malloc(sizeof(uint16_t)*new_morph_lod.mVertexCount));
            read(file, new_morph_lod.mVertexLinks, sizeof(uint16_t)*new_morph_lod.mVertexCount);

            new_morph.mLODs[i] = new_morph_lod;
        }
        
        read(file, &new_morph.mMinBox);
        read(file, &new_morph.mMaxBox);
        read(file, new_morph.mUnk1, sizeof(float) * 4);

    }
    return new_morph;
}

DataFormat4DS::SingleMeshLodJoint DataFormat4DS::loadSingleMeshLodJoint(std::ifstream &file)
{
    SingleMeshLodJoint new_joint = {};
    read(file, &new_joint.mTransform);
    read(file, &new_joint.mUnk0, sizeof(uint32_t));
    read(file, &new_joint.mAdditionalValuesCount);
    read(file, &new_joint.mBoneID);
    read(file, &new_joint.mMinBox);
    read(file, &new_joint.mMaxBox);

    new_joint.mAdditionalValues = reinterpret_cast<float*>(malloc(sizeof(float)*new_joint.mAdditionalValuesCount));
    read(file, new_joint.mAdditionalValues, sizeof(float)*new_joint.mAdditionalValuesCount);
   
    return new_joint;
}

DataFormat4DS::SingleMeshLod DataFormat4DS::loadSingleMeshLod(std::ifstream &file)
{
    SingleMeshLod new_lod = {};
    read(file, &new_lod.mJointCount);
    read(file, &new_lod.mUnk0);
    read(file, &new_lod.mMinBox);
    read(file, &new_lod.mMaxBox);

    new_lod.mJoints = reinterpret_cast<SingleMeshLodJoint*>(malloc(sizeof(SingleMeshLodJoint)*new_lod.mJointCount));
    for(size_t i = 0; i < new_lod.mJointCount; ++i) 
    {
        SingleMeshLodJoint new_joint = {};
        new_joint = loadSingleMeshLodJoint(file);
        new_lod.mJoints[i] = new_joint;
    }  
      
    return new_lod;
}

DataFormat4DS::SingleMesh DataFormat4DS::loadSingleMesh(std::ifstream &file)
{
    SingleMesh new_mesh = {};
    
    new_mesh.mStandard = loadStandard(file);
    
    new_mesh.mLODs = reinterpret_cast<SingleMeshLod*>(malloc(sizeof(SingleMeshLod)*new_mesh.mStandard.mLODLevel));
    
    for(size_t i = 0; i < new_mesh.mStandard.mLODLevel; ++i) 
    {
        SingleMeshLod new_lod = {};
        new_lod = loadSingleMeshLod(file);
        new_mesh.mLODs[i] = new_lod;
    }
    
    return new_mesh;
}

DataFormat4DS::SingleMorph DataFormat4DS::loadSingleMorph(std::ifstream &file)
{
    SingleMorph new_morph = {};
    new_morph.mSingleMesh = loadSingleMesh(file);

    new_morph.mMorph = loadMorph(file, 1);
    return new_morph;
}

void DataFormat4DS::loadMesh(Model *model, std::ifstream &file)
{
    read(file, &model->mMeshCount);
    model->mMeshes = reinterpret_cast<Mesh*>(malloc(sizeof(Mesh)*model->mMeshCount));

    for(size_t i = 0; i < model->mMeshCount; ++i)
    {
        Mesh new_mesh = {};
        read(file, &new_mesh.mMeshType);

        if(new_mesh.mMeshType == MESHTYPE_STANDARD)
        {
            read(file, &new_mesh.mVisualMeshType);
            read(file, &new_mesh.mMeshRenderFlags);
        }

        read(file, &new_mesh.mParentID);
        read(file, &new_mesh.mPos);
        read(file, &new_mesh.mScale);

        float r[4];
        read(file, r, sizeof(float) * 4);
        new_mesh.mRot = {r[1], r[2], r[3], -r[0]};

        read(file, &new_mesh.mCullingFlags);
        read(file, &new_mesh.mMeshNameLength);
        read(file, new_mesh.mMeshName, new_mesh.mMeshNameLength);
        read(file, &new_mesh.mMeshParamsLength);
        read(file, new_mesh.mMeshParams, new_mesh.mMeshParamsLength);

        switch(new_mesh.mMeshType)
        {
            case MESHTYPE_STANDARD:
            {
                switch(new_mesh.mVisualMeshType)
                {
                    case VISUALMESHTYPE_STANDARD:
                    {
                        Standard new_standard = {};
                        new_standard = loadStandard(file);
                        new_mesh.mStandard = new_standard;
                    } 
                    break;

                    case VISUALMESHTYPE_MIRROR:
                    {
                        Mirror new_mirror = {};
                        new_mirror = loadMirror(file);
                        new_mesh.mMirror = new_mirror;
                    } 
                    break;

                    case VISUALMESHTYPE_GLOW:
                    {
                        Glow new_glow = {};
                        new_glow = loadGlow(file);
                        new_mesh.mGlow = new_glow;
                    } 
                    break;

                    case VISUALMESHTYPE_BILLBOARD:
                    {
                        Billboard new_billboard = {};
                        new_billboard.mStandard = loadStandard(file);
                        read(file, &new_billboard.mRotationAxis);
                        read(file, &new_billboard.mIgnoreCamera);
                        new_mesh.mBillboard = new_billboard;
                    } 
                    break;
                    
                    case VISUALMESHTYPE_MORPH:
                    {
                        Morph new_morph = {};
                        new_morph = loadMorph(file, 0);
                        new_mesh.mMorph = new_morph;
                    }
                    break;
                                
                    case VISUALMESHTYPE_SINGLEMESH:
                    {
                        SingleMesh new_single_mesh = {};
                        new_single_mesh = loadSingleMesh(file);
                        new_mesh.mSingleMesh = new_single_mesh;
                    }
                    break;
                    
                    case VISUALMESHTYPE_SINGLEMORPH:
                    {
                        SingleMorph new_single_morph = {};
                        new_single_morph = loadSingleMorph(file);
                        new_mesh.mSingleMorph = new_single_morph;
                    }
                    break;

                    default:
                    {
                    }
                    break;
                }
            }
            break;

            case MESHTYPE_DUMMY:
            {
                Dummy new_dummy = {};
                read(file, &new_dummy.mMinBox);
                read(file, &new_dummy.mMaxBox);
                new_mesh.mDummy = new_dummy;
            }
            break;
            
            case MESHTYPE_SECTOR:
            {
                Sector new_sector = {};
                new_sector = loadSector(file);
                new_mesh.mSector = new_sector;
            }
            break;
            
            case MESHTYPE_TARGET:
            {
                Target new_target = {};
                new_target = loadTarget(file);
                new_mesh.mTarget = new_target;
            }
            break;
            
            case MESHTYPE_BONE:
            {
                Bone new_bone = {};
                read(file, &new_bone.mTransform);
                read(file, &new_bone.mBoneID);
                new_mesh.mBone = new_bone;
            }
            break;

            default: 
            {
            }
            break;
        }

        model->mMeshes[i] = new_mesh;
    }
}

DataFormat4DS::Model* DataFormat4DS::loadModel(std::ifstream &file)
{
    Model *model = reinterpret_cast<Model*>(malloc(sizeof(Model)));
    read(file, &model->mSignature, 4);

    //TODO(DavoSK): Add check for proper format signature contains 4DS word in ASCII 
    read(file, &model->mFormatVersion);
    read(file, &model->mTimestamp);
    
    loadMaterial(model, file);
    loadMesh(model, file);
    read(file, &model->mUse5DS);
    
    return model;
}

bool DataFormat4DS::load(std::ifstream &srcFile)
{
    mLoadedModel = loadModel(srcFile);
    return mLoadedModel != nullptr;
}

}

#endif
