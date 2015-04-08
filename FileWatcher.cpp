#include "FileWatcher.h"

namespace FileWatcher
{

FileWatcher::FileWatcher()
{
    baseInstance = new WatchInstance();
}

FileWatcher::~FileWatcher()
{
    delete baseInstance;
}

bool FileWatcher::displayDirectoryTree(std::string directory)
{
    if(directory.c_str()[directory.length()-1] == '/')
        return baseInstance->platformDisplayDirectory(directory);
    else return baseInstance->platformDisplayDirectory(directory + "/*");
}

void FileWatcher::print(std::string text, std::initializer_list<Colors> colors)
{
    baseInstance->platformPrintColorS(text, colors);
}

WatchInstanceHandle FileWatcher::addNewWatchInstance(std::string directory)
{
    WatchInstanceHandle k = allocInstance.getHandle();
    if(k != NULL_HANDLE)
    {

        WatchInstance* instance = new WatchInstance(directory);
        instance->platformBeginDirectoryWatch();
        directoryNames.insert(std::make_pair(k, instance));
    }
    return k;
}

void FileWatcher::stopWatchInstance(WatchInstanceHandle handle)
{
    directoryNames.at(handle)->platformCloseThread();
    delete directoryNames.at(handle);
    directoryNames.erase(handle);
    allocInstance.remove(handle);
}

void FileWatcher::removeWatchEvent(WatchInstanceHandle dirHandle, WatchEventHandle eventHandle)
{

    if(directoryNames.find(dirHandle) != directoryNames.end())
    {
        directoryNames.at(dirHandle)->removeEvent(eventHandle);
    }
}

void FileWatcher::wait(unsigned milliseconds = 0)
{
    baseInstance->platformThreadedWait(milliseconds);
}
}
