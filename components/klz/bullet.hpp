#ifndef TREE_KLZ_BULLET_LOADER_H
#define TREE_KLZ_BULLET_LOADER_H

#include <vector>
#include <fstream>
#include <bullet_utils.hpp>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btCylinderShape.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <klz/parser.hpp>

namespace MFPhysics
{

class BulletTreeKlzLoader
{
public:
    typedef struct
    {
        unsigned int mI1;
        unsigned int mI2;
        unsigned int mI3;
    } FaceIndices;

    typedef struct
    {
        std::string mMeshName;
        std::vector<FaceIndices> mFaces;
    } MeshFaceCollision;

    void load(std::ifstream &srcFile);

    std::vector<MFUtil::NamedRigidBody> mRigidBodies;
    std::vector<MeshFaceCollision> mFaceCollisions;
};

void BulletTreeKlzLoader::load(std::ifstream &srcFile)
{
    MFFormat::DataFormatTreeKLZ klz;
    klz.load(srcFile);

    std::vector<std::string> linkStrings = klz.getLinkStrings();

    #define loopBegin(getFunc) \
    { \
        auto cols = klz.getFunc(); \
        for (int i = 0; i < (int) cols.size(); ++i) \
        { \
            auto col = cols[i];\
            MFUtil::NamedRigidBody newBody;\

    #define loopEnd \
            newBody.mName = linkStrings[cols[i].mLink]; \
            mRigidBodies.push_back(newBody); \
        } \
    }

    loopBegin(getAABBCols)
        btVector3 p1 = MFUtil::mafiaVec3ToBullet(col.mMin.x,col.mMin.y,col.mMin.z);
        btVector3 p2 = MFUtil::mafiaVec3ToBullet(col.mMax.x,col.mMax.y,col.mMax.z);

        btVector3 center = (p1 + p2) / 2.0f;
        btVector3 bboxCorner = p2 - center;

        newBody.mRigidBody.mShape = std::make_shared<btBoxShape>(bboxCorner);
        btRigidBody::btRigidBodyConstructionInfo ci(0,0,newBody.mRigidBody.mShape.get());
        newBody.mRigidBody.mBody = std::make_shared<btRigidBody>(ci);
        newBody.mRigidBody.mBody->translate(center);
    loopEnd

    loopBegin(getSphereCols)
        btVector3 center = MFUtil::mafiaVec3ToBullet(col.mPosition.x,col.mPosition.y,col.mPosition.z);
        float radius = col.mRadius;

        newBody.mRigidBody.mShape = std::make_shared<btSphereShape>(radius);
        btRigidBody::btRigidBodyConstructionInfo ci(0,0,newBody.mRigidBody.mShape.get());
        newBody.mRigidBody.mBody = std::make_shared<btRigidBody>(ci);
        newBody.mRigidBody.mBody->translate(center);
    loopEnd

    #define loadOBBOrXTOBB \
        btVector3 p1 = MFUtil::mafiaVec3ToBullet(col.mExtends[0].x,col.mExtends[0].y,col.mExtends[0].z); \
        btVector3 p2 = MFUtil::mafiaVec3ToBullet(col.mExtends[1].x,col.mExtends[1].y,col.mExtends[1].z); \
        btVector3 center = (p1 + p2) / 2.0f; \
        btVector3 bboxCorner = p2 - center; \
        MFFormat::DataFormat::Vec3 trans = col.mTransform.getTranslation(); \
        MFFormat::DataFormat::Mat4 rot = col.mTransform; \
        rot.separateRotation(); \
        btMatrix3x3 rotMat; \
        rotMat.setValue(rot.a0,rot.a1,rot.a2,rot.b0,rot.b1,rot.b2,rot.c0,rot.c1,rot.c2); \
        btTransform transform(rotMat,MFUtil::mafiaVec3ToBullet(trans.x,trans.y,trans.z)); \
        btQuaternion q = transform.getRotation(); \
        transform.setRotation(btQuaternion(q.x(),q.z(),q.y(),q.w()));   /* TODO: find out why Y and Z have to be switched here */ \
        /* note: scale seems to never be used */ \
        newBody.mRigidBody.mShape = std::make_shared<btBoxShape>(bboxCorner); \
        btRigidBody::btRigidBodyConstructionInfo ci(0,0,newBody.mRigidBody.mShape.get()); \
        newBody.mRigidBody.mBody = std::make_shared<btRigidBody>(ci); \
        newBody.mRigidBody.mBody->setWorldTransform(transform);

    loopBegin(getOBBCols)
        loadOBBOrXTOBB
    loopEnd

    loopBegin(getXTOBBCols)
        loadOBBOrXTOBB
    loopEnd
    
    loopBegin(getCylinderCols)
        btVector3 center = btVector3(col.mPosition.x,col.mPosition.y,0);
        float radius = col.mRadius;

        newBody.mRigidBody.mShape = std::make_shared<btCylinderShapeZ>(btVector3(radius,0,50.0));  // FIXME: cylinder height infinite?
        btRigidBody::btRigidBodyConstructionInfo ci(0,0,newBody.mRigidBody.mShape.get());
        newBody.mRigidBody.mBody = std::make_shared<btRigidBody>(ci);
        newBody.mRigidBody.mBody->translate(center);
    loopEnd

    // load face collisions:

    auto cols = klz.getFaceCols();

    int currentLink = -1;
    MeshFaceCollision faceCol;

    // TODO: the following loop supposes the cols are grouped by link, find out whether that is always true

    for (int i = 0; i < (int) cols.size(); ++i)
    {
        auto col = cols[i];

        FaceIndices face;
        face.mI1 = col.mIndices[0].mIndex;
        face.mI2 = col.mIndices[1].mIndex;
        face.mI3 = col.mIndices[2].mIndex;

        if (currentLink < 0 || currentLink != col.mIndices[0].mLink)
        {
            // start loading a new mesh
            
            if (currentLink >= 0)
                mFaceCollisions.push_back(faceCol);

            faceCol.mFaces.clear();
            currentLink = col.mIndices[0].mLink;

            faceCol.mMeshName = linkStrings[currentLink];
        }

        faceCol.mFaces.push_back(face);

        if (i == ((int) cols.size()) - 1)   // last one => push
            mFaceCollisions.push_back(faceCol);
    }
}

}

#endif
