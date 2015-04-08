#include "FileWatcherBase.h"

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
