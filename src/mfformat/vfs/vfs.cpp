// MafiaHub - https://github.com/MafiaHub //

/** 
* @file mfformat/vfs/vfs.cpp
* @author Dominik Madarász <zaklaus@outlook.com>	
* @author Miloslav Číž <tastyfish@seznam.com>	
**/
#include <mfformat/vfs/vfs.hpp>
#include <mfformat/utils/openmf.hpp>

#ifdef OMF_SYSTEM_LINUX
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

namespace MFFile
{
    
std::string convertPathToCanonical(std::string path)
{
    return MFUtil::strToLower(path);
}

FileSystem::FileSystem()
{
    mSearchPaths.push_back("resources");
    mSearchPaths.push_back("mafia");
    
    char const *envMafiaDir = getenv(MAFIA_INSTALL_DIR); // TODO: Use Config for this.
    if (envMafiaDir)
        mSearchPaths.push_back(envMafiaDir);

    #if defined(OMF_SYSTEM_LINUX)
    struct passwd *pw = getpwuid(getuid());
    const char *dir = pw->pw_dir;
    std::string dirpath = std::string(dir) + "/";
    addPath(dirpath + ".openmf");
    addPath(dirpath + ".openmf/mafia");
    #endif
}

void FileSystem::prependPath(std::string path)
{
	addPath(path, 0);
}

void FileSystem::addPath(std::string path)
{
	addPath(path, mSearchPaths.size());
}

void FileSystem::addPath(std::string path, size_t index)
{
    if (path[path.length() - 1] == '/')
        path.erase(path.length() - 1,1);            

	mSearchPaths.insert(mSearchPaths.begin() + index, path);
}

std::string FileSystem::getFileLocation(const std::string& fileName)
{
    for (const auto& path : mSearchPaths)     // TODO: what's the fastest way to check a file existence?
    {
        std::string realPath = path + "/";
        realPath.append(fileName);

        std::ifstream f;
        f.open(realPath);

        if (f.good())
        {
            f.close();
            return realPath;
        }
    }

    return "";
}

bool FileSystem::open(std::ifstream &file, std::string fileName, std::ios_base::openmode /*mode*/)
{
    fileName = convertPathToCanonical(fileName);

    std::string fileLocation = getFileLocation(fileName);    

    if (fileLocation.length() == 0)
    {
        MFLogger::Logger::warn("Could not open file: " + fileName + ".",VFS_MODULE_STR);
        return false;
    }

    file.open(fileLocation, std::ios::binary);
    return file.good();
}

}
