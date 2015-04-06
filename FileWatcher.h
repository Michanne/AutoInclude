#ifndef H_FILE_WATCHER_H
#define H_FILE_WATCHER_H
#pragma once

#include "windows/FileWatcherWindows.h"
#include <unordered_map>

#if _WIN32
#define USING_WINDOWS
#elif defined(__APPLE__) || defined(__MACH__)
#define USING_MACOSX
#elif __linux__
#define USING_LINUX
#endif // OS

class FileWatcher
    :
    #if defined(USING_WINDOWS)
        public FileWatcherBaseWindows
    #elif defined(USING_MACOSX)
    #elif defined(USING_LINUX)
    #endif // defined
{

public:

    FileWatcher(std::string);
    FileWatcher(){};
    ~FileWatcher();

    void close();
    void wait();
    void print(const char*, std::initializer_list<Colors>);
    void print(std::string, std::initializer_list<Colors>);
    std::string getCurrentDirectory();
};

#endif // H_FILE_WATCHER_H
