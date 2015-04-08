#include "FileWatcherBase.h"

std::string FileWatcherBase::queryForShorthandName(std::string originalFilename)
{
    std::string temp;
    platformPrintColorS("What name would you like to give to '" + originalFilename + "'?", {Colors::BLUE, Colors::GREEN, Colors::B_BLUE, Colors::B_INTENSE});
    std::cout << std::endl;
    std::getline(std::cin, temp);
    return temp;
}

void FileWatcherBase::handleFileState(FileHandleState fhs, std::string filename)
{
    char* yesno;

    switch(fhs)
    {
    case FileHandleState::ADDED:
        platformPrintColorS(filename + " was created in the directory. Would you like to add the file to this autoinclude project?",
                            {Colors::GREEN});
        std::cin.get(yesno, 1);
        if(yesno[0] == 'y')
            queryForShorthandName(filename);
            break;
    case FileHandleState::REMOVED:
        platformPrintColorS(filename + " was removed from the directory. Removing it from the autoinclude directory...",
                            {Colors::RED});
            break;

    }
}

void FileWatcherBase::removeEvent(WatchEventHandle handle)
{
    allocEvent.remove(handle);
}

WatchInstanceHandle WatchHandleAllocator::getHandle()
{

    if(freeIndices.size() > 0)
    {

        WatchInstanceHandle k = freeIndices.front();
        usedIndices[k] = true;
        freeIndices.pop();
        ++size;
        return k;
    }

    else
    {
        if(size >= WATCH_HANDLE_ALLOCATOR_MAX_SIZE-1)
            return NULL_HANDLE;
        else
        {
            usedIndices[size-1] = true;
            return size-1;
        }
    }
}

void WatchHandleAllocator::remove(WATCH_HANDLE handle)
{
    usedIndices[handle] = false;
    freeIndices.push(handle);
}

bool WatchHandleAllocator::okay(WATCH_HANDLE handle)
{
    return usedIndices[handle];
}
