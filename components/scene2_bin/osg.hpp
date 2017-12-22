#ifndef OSG_SCENE2_LOADER_H
#define OSG_SCENE2_LOADER_H

#include <osg/Node>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osg/ShapeDrawable>
#include <osg/Light>
#include <osg/LightSource>
#include <fstream>
#include <algorithm>
#include <scene2_bin/parser.hpp>
#include <loggers/console.hpp>
#include <osg/Material>
#include <utils.hpp>
#include <osg_utils.hpp>
#include <base_loader.hpp>
#include <osgText/Text3D>
#include <osgText/Font3D>
#include <osg/Billboard>

namespace MFFormat
{

class OSGScene2BinLoader : public OSGLoader
{
public:
    osg::ref_ptr<osg::Node> load(std::ifstream &srcFile, std::string fileName="");
};

osg::ref_ptr<osg::Node> OSGScene2BinLoader::load(std::ifstream &srcFile, std::string fileName)
{
    osg::ref_ptr<osg::Group> group = new osg::Group();

    MFLogger::ConsoleLogger::info("loading scene2.bin");

    MFFormat::DataFormatScene2BIN parser;

    MFFormat::OSG4DSLoader loader4DS;
    loader4DS.setBaseDir(mBaseDir);

    bool success = parser.load(srcFile);

    if (success)
    {
        std::map<std::string,osg::ref_ptr<osg::Group>> nodeMap;

        osg::ref_ptr<MFUtil::MoveEarthSkyWithEyePointTransform> cameraRel = new
        MFUtil::MoveEarthSkyWithEyePointTransform();   // for Backdrop sector (camera relative placement)

        cameraRel->setCullingActive(false);

        // disable lights for backdrop sector:
        osg::ref_ptr<osg::Material> mat = new osg::Material;

        mat->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4f(1,1,1,1));
        mat->setColorMode(osg::Material::OFF);
        cameraRel->getOrCreateStateSet()->setAttributeAndModes(mat,
        osg::StateAttribute::ON |
        osg::StateAttribute::OVERRIDE);

        osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
        trans->setMatrix(osg::Matrixd::scale(osg::Vec3f(10,10,10)));   // so that the skybox doesn't cut the scene off
        trans->addChild(cameraRel);

        group->addChild(trans);
 
        unsigned int lightNumber = 0;
     
        // TMP: add default light until game lights are correctly handled
        osg::ref_ptr<osg::LightSource> defaultLightNode = new osg::LightSource();
        defaultLightNode->getLight()->setPosition( osg::Vec4f(1,1,1,0) );
        defaultLightNode->getLight()->setLightNum( lightNumber++ );
        defaultLightNode->getLight()->setAmbient( osg::Vec4f(0.5,0.5,0.5,1.0) );
        group->addChild( defaultLightNode );

        for (auto pair : parser.getObjects())
        {
            auto object = pair.second;

            osg::ref_ptr<osg::Node> objectNode;

            std::string logStr = object.mName + ": ";

            bool hasTransform = true;

            switch (object.mType)
            {
                case MFFormat::DataFormatScene2BIN::OBJECT_TYPE_LIGHT:
                {
                    if (object.mLightType != MFFormat::DataFormatScene2BIN::LIGHT_TYPE_POINT)
                        break;

                    logStr += "light";

                    #if 0
                        // for debug
                        osg::ref_ptr<osg::ShapeDrawable> lightNode = new osg::ShapeDrawable(
                        new osg::Sphere(osg::Vec3f(0,0,0),0.1));
                    #else
                        if (lightNumber > 6)      // fixed pipeline only supports 7 lights
                            break;

                        osg::ref_ptr<osg::LightSource> lightNode = new osg::LightSource();

                        MFFormat::DataFormat::Vec3 c = object.mLightColour;
                        osg::Vec3f lightColor = osg::Vec3f(c.x,c.z,c.z) * object.mLightPower;

                        lightNode->getLight()->setDiffuse(osg::Vec4(lightColor,1));
                        lightNode->getLight()->setAmbient(osg::Vec4(lightColor * 0.05,1));
                        lightNode->getLight()->setSpecular(osg::Vec4(1,1,1,1));

                        bool isPositional =
                            object.mLightType == MFFormat::DataFormatScene2BIN::LIGHT_TYPE_POINT ||
                            object.mLightType == MFFormat::DataFormatScene2BIN::LIGHT_TYPE_POINT_AMBIENT;

                        osg::Vec3f lightPos = isPositional ?
                            osg::Vec3f(0,0,0) :             // position will be set via transform node
                            toOSG(object.mPos);             // for directional lights position usually defines direction - is this the right field though?

                        lightNode->getLight()->setPosition( osg::Vec4f(lightPos,
                            isPositional ? 1.0f : 0.0f) );  // see OpenGL light types
 
                        lightNode->getLight()->setLightNum(lightNumber);         // each light must have a unique number
                        lightNumber++;
                    #endif

                    objectNode = lightNode;
                    break;
                }

                case MFFormat::DataFormatScene2BIN::OBJECT_TYPE_MODEL:
                {
                    logStr += "model: " + object.mModelName;

                    objectNode = (osg::Node *) getFromCache(object.mModelName).get();

                    if (!objectNode)
                    {
                        std::ifstream f;
                        
                        if (!mFileSystem->open(f,"models/" + object.mModelName))
                        {
                            MFLogger::ConsoleLogger::warn("Could not load model " + object.mModelName + ".");
                        }
                        else
                        {
                            objectNode = loader4DS.load(f,object.mModelName);   
                            storeToCache(object.mModelName,objectNode);
                            f.close();
                        }
                    }

                    break;
                }

                default:
                {
                    logStr += "unknown";
                    hasTransform = false;
                    break;
                }
            }
            
            MFLogger::ConsoleLogger::info(logStr);

            if (objectNode.get())
            {
                osg::ref_ptr<osg::MatrixTransform> objectTransform = new osg::MatrixTransform();

                if (hasTransform)
                {
                    osg::Matrixd m = makeTransformMatrix(object.mPos,object.mScale,object.mRot);
                    m.preMult( osg::Matrixd::rotate(osg::PI,osg::Vec3f(1,0,0)) );
                    objectTransform->setMatrix(m);
                }

                objectTransform->addChild(objectNode);
                objectTransform->setName(object.mParentName);    // hack: store the parent name in node name
                nodeMap.insert(nodeMap.begin(),std::pair<std::string,osg::ref_ptr<osg::Group>>(object.mName,objectTransform));
            }
        }   // for

        for (auto pair : nodeMap)   // set parents
        {
            std::string parentName = pair.second->getName();

            if (parentName.compare("Backdrop sector") == 0)      // backdrop is for camera-relative stuff
                cameraRel->addChild(pair.second);
            else if ( nodeMap.find(parentName) != nodeMap.end() )
                nodeMap[parentName]->addChild(pair.second);
            else
                group->addChild(pair.second);

            pair.second->setName("");
        }
    }

    return group;
}

}

#endif
