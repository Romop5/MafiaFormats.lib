#ifndef SPATIAL_ENTITY_FACTORY
#define SPATIAL_ENTITY_FACTORY

#include <osg/Group>
#include <physics/bullet_physics_world.hpp>
#include <spatial_entity/spatial_entity_implementation.hpp>

#define SPATIAL_ENTITY_FACTORY_MODULE_STR "spatial entity factory"

namespace MFGame
{

class SpatialEntityFactory
{
public:
    SpatialEntityFactory(MFRender::OSGRenderer *renderer, MFPhysics::BulletPhysicsWorld *physicsWorld, MFGame::SpatialEntityManager *entityManager);
    void createMissionEntities();

    SpatialEntity::Id createEntity(
        osg::MatrixTransform *graphicNode,
        std::shared_ptr<btRigidBody> physicsBody=0,
        std::shared_ptr<btDefaultMotionState> physicsMotionsState=0, 
        std::string name="");

    SpatialEntity::Id createTestBallEntity();

protected:
    MFGame::SpatialEntityManager *mEntityManager;
    MFPhysics::BulletPhysicsWorld *mPhysicsWorld;
    MFRender::OSGRenderer *mRenderer;

    osg::ref_ptr<osg::Shape> mTestVisualSphereShape;
    osg::ref_ptr<osg::ShapeDrawable> mTestSphereNode;

    std::shared_ptr<btCollisionShape> mTestPhysicalSphereShape;
};

SpatialEntityFactory::SpatialEntityFactory(MFRender::OSGRenderer *renderer, MFPhysics::BulletPhysicsWorld *physicsWorld, MFGame::SpatialEntityManager *entityManager)
{
    mRenderer = renderer;
    mPhysicsWorld = physicsWorld;
    mEntityManager = entityManager;

    const double r = 0.7;

    mTestVisualSphereShape = new osg::Sphere(osg::Vec3f(0,0,0),r);
    mTestSphereNode = new osg::ShapeDrawable(mTestVisualSphereShape);

    mTestPhysicalSphereShape = std::make_shared<btSphereShape>(r);
}

SpatialEntity::Id SpatialEntityFactory::createEntity(
    osg::MatrixTransform *graphicNode,
    std::shared_ptr<btRigidBody> physicsBody,
    std::shared_ptr<btDefaultMotionState> physicsMotionsState, 
    std::string name)
{
    std::shared_ptr<MFGame::SpatialEntityImplementation> newEntity = std::make_shared<MFGame::SpatialEntityImplementation>();
    newEntity->setName(name);
    newEntity->setOSGRootNode(mRenderer->getRootNode());
    newEntity->setVisualNode(graphicNode);
    newEntity->setPhysicsBody(physicsBody);
    newEntity->setPhysicsMotionState(physicsMotionsState);
    newEntity->ready();
    mEntityManager->addEntity(newEntity);
    return newEntity->getId();
}

SpatialEntity::Id SpatialEntityFactory::createTestBallEntity()
{
    osg::ref_ptr<osg::MatrixTransform> visualNode = new osg::MatrixTransform();
    visualNode->addChild(mTestSphereNode);
    mRenderer->getRootNode()->addChild(visualNode);

    std::shared_ptr<btDefaultMotionState> motionState = std::make_shared<btDefaultMotionState>();

    btScalar mass = 1;
    btVector3 fallInertia(0, 0, 0);
    mTestPhysicalSphereShape->calculateLocalInertia(mass,fallInertia);
    btRigidBody::btRigidBodyConstructionInfo ci(mass,motionState.get(),mTestPhysicalSphereShape.get(),fallInertia);

    std::shared_ptr<btRigidBody> physicalBody = std::make_shared<btRigidBody>(ci);
    mPhysicsWorld->getWorld()->addRigidBody(physicalBody.get());
    return createEntity(visualNode.get(), physicalBody, motionState, "test sphere");
}

class CreateEntitiesFromSceneVisitor: public osg::NodeVisitor
{
public:
    CreateEntitiesFromSceneVisitor(std::vector<MFUtil::NamedRigidBody> *treeKlzBodies, MFGame::SpatialEntityFactory *entityFactory): osg::NodeVisitor()
    {
        mTreeKlzBodies = treeKlzBodies;
        mEntityFactory = entityFactory;
        mModelName = "";

        for (int i = 0; i < (int) treeKlzBodies->size(); ++i)
            mNameToBody.insert(std::pair<std::string,MFUtil::NamedRigidBody *>((*treeKlzBodies)[i].mName,&((*treeKlzBodies)[i])));
    }

    virtual void apply(osg::Node &n) override
    {
        MFUtil::traverse(this,n);
    }

    virtual void apply(osg::MatrixTransform &n) override
    {
        std::string modelName = "";

        if (n.getUserDataContainer())
        {
            std::vector<std::string> descriptions = n.getUserDataContainer()->getDescriptions();

            if (descriptions.size() > 0)
            {
                if (descriptions[0].compare("scene2.bin model") == 0)
                {
                    modelName = n.getName();
                }
                else if (descriptions[0].compare("4ds mesh") == 0)
                {
                    // find the corresponding collision:
    
                    MFUtil::NamedRigidBody *matchedBody = 0;

                    auto findResult = mNameToBody.find(n.getName());

                    if (findResult != mNameToBody.end())
                    {
                        matchedBody = findResult->second;
                    }
                    else
                    {
                        findResult = mNameToBody.find(mModelName + "." + n.getName());

                        if (findResult != mNameToBody.end())
                            matchedBody = findResult->second;
                    }

                    if (!matchedBody)
                        MFLogger::ConsoleLogger::warn("Could not find matching collision for visual node \"" + n.getName() + "\".",SPATIAL_ENTITY_FACTORY_MODULE_STR);

                    mEntityFactory->createEntity(&n, matchedBody ? matchedBody->mRigidBody.mBody : 0, 0, n.getName());
                }
            }
        }

        if (modelName.size() > 0)
            mModelName = modelName;          // traverse downwards with given name prefix

        MFUtil::traverse(this,n);

        if (modelName.size() > 0)
            mModelName = "";                 // going back up => clear the name prefix
    }

    std::set<std::string> mMatchedBodies;

protected:
    std::vector<MFUtil::NamedRigidBody> *mTreeKlzBodies;
    MFGame::SpatialEntityFactory *mEntityFactory;
    std::string mModelName;                  // when traversin into a model loaded from scene2.bin, this will contain the model name (needed as the name prefix)
    std::map<std::string,MFUtil::NamedRigidBody *> mNameToBody;

    bool namesMatch(std::string nameVisual, std::string nameCol, std::string nameModel)
    {
        if (nameVisual.compare(nameCol) == 0)
            return true;

        if (nameVisual.compare(nameModel + "." + nameCol) == 0)
            return true;

        if (nameCol.compare(nameModel + "." + nameVisual) == 0)
            return true;

        return false;
    }
};

void SpatialEntityFactory::createMissionEntities()
{
    auto treeKlzBodies = mPhysicsWorld->getTreeKlzBodies();

    CreateEntitiesFromSceneVisitor v(&treeKlzBodies,this);
    mRenderer->getRootNode()->accept(v);

    // process the unmatched rigid bodies:

    for (int i = 0; i < (int) treeKlzBodies.size(); ++i)
    {
        if (v.mMatchedBodies.find(treeKlzBodies[i].mName) != v.mMatchedBodies.end())
            continue;

        createEntity(0,treeKlzBodies[i].mRigidBody.mBody,0,treeKlzBodies[i].mName);
    }

    // TODO: set the static flag to the loaded bodies here
}

}

#endif
