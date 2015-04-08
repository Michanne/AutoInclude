#ifndef H_FILE_WATCHER_H
#define H_FILE_WATCHER_H
#pragma once

#include <unordered_map>
#include <sstream>
#include <functional>

#if _WIN32
#define USING_WINDOWS
#include "windows/FileWatcherWindows.h"
#elif defined(__APPLE__) || defined(__MACH__)
#define USING_MACOSX
#elif __linux__
#define USING_LINUX
#else
#define USING_WINDOWS
#include "windows/FileWatcherWindows.h"
#endif // OS

namespace FileWatcher
{

class FileWatcher
{

private:
    std::unordered_map<WatchInstanceHandle, WatchInstance*> directoryNames;

    WatchInstance* baseInstance;
    WatchHandleAllocator allocInstance;

public:

    FileWatcher();
    ~FileWatcher();

    WatchInstanceHandle addNewWatchInstance(std::string);
    void stopWatchInstance(WatchInstanceHandle);

    template <typename FUNCTION> WatchEventHandle delegateNewWatchEvent(WatchInstanceHandle, WatchFileOption, FUNCTION);
    void removeWatchEvent(WatchInstanceHandle, WatchEventHandle);

    std::string getCurrentWatchedDirectory(WatchInstanceHandle);
    bool displayDirectoryTree(std::string);
    void print(std::string, std::initializer_list<Colors>);
    WatchEventInformation getEventInformation(WatchEventHandle);
    void wait(unsigned);
};
#include "FileWatcher.tcc"
}

#endif // H_FILE_WATCHER_H
