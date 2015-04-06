#include "FileWatcher.h"

FileWatcher::FileWatcher(std::string directoryName)
:
    #ifdef USING_WINDOWS
    FileWatcherBaseWindows(directoryName.c_str())
    #endif //constructor
{

    print("Watching directory " + directoryName + "...\n",
          {Colors::RED, Colors::GREEN});
}

FileWatcher::~FileWatcher()
{

}

void FileWatcher::close()
{
    platformWaitThread();
}

void FileWatcher::wait()
{
    platformHoldForQuit();
}

void FileWatcher::print(std::string text, std::initializer_list<Colors> colors)
{
    platformPrintColorS(text, colors);
}

void FileWatcher::print(const char* text, std::initializer_list<Colors> colors)
{
    platformPrintColorS(text, colors);
}

std::string FileWatcher::getCurrentDirectory()
{
    return platformQueryDirectory();
}


