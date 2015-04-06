#include "FileWatcher.h"

#if defined(USING_WINDOWS)
    #include <windows.h>
#elif defined(USING_MACOSX)

#endif // includes

FileWatcher::FileWatcher()
{

}

FileWatcher::~FileWatcher()
{

}

const char* FileWatcher::generateMainHeader()
{

#if defined(USING_WINDOWS)

#elif defined(USING_MACOSX)

#elif defined(USING_LINUX)

#endif // gen header

}


