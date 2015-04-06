#ifndef H_FILE_WATCHER_H
#define H_FILE_WATCHER_H
#pragma once

#include "windows/FileWatcherWindows.h"
#include <unordered_map>

enum FileState
{
    REMOVED,
    MODIFIED,
    ADDED,
    UNCHANGED
};

struct FileWatcherConfig
{

    const char*                                 currentlyWatchedDirectory;
    std::unordered_map<const char*, FileState>  currentlyWatchedFiles;

};

class FileWatcher
    : public
    #if defined(USING_WINDOWS)
        FileWatcherBaseWindows
    #elif defined(USING_MACOSX)
    #elif defined(USING_LINUX)
    #endif // defined
{

private:

    FileWatcherConfig   config;

    const char* generateMainHeader();
    const char* generateHeaderFromURL();
    const char* generateHeaderFromImplementation();

public:

    FileWatcher();
    ~FileWatcher();

    void watchFile();
    void watchHeader();
    void autoInclude();
    void traverseDirectory();

    void setPollFrequency();
};

#endif // H_FILE_WATCHER_H
