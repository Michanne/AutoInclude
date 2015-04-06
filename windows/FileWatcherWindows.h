#ifndef H_FILE_WATCHER_WINDOWS_BASE_H
#define H_FILE_WATCHER_WINDOWS_BASE_H
#pragma once

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

class FileWatcherBaseWindows
{

private:

    FileWatcherConfig   config;

    const char* generateMainHeader();
    const char* generateHeaderFromURL();
    const char* generateHeaderFromImplementation();

public:

    FileWatcher();
    ~FileWatcher();
};
