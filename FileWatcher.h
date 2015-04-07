#ifndef H_FILE_WATCHER_H
#define H_FILE_WATCHER_H
#pragma once

#include "windows/FileWatcherWindows.h"
#include <chrono>
#include <sstream>

#if _WIN32
#define USING_WINDOWS
#elif defined(__APPLE__) || defined(__MACH__)
#define USING_MACOSX
#elif __linux__
#define USING_LINUX
#endif // OS

enum Commands
{
    INVALID,
    SELECT,
    VIEW,
    PRINT_CURRENT_DIRECTORY,
    CHANGE_ABSOLUTE_DIRECTORY,
    CHANGE_RELATIVE_DIRECTORY,
    QUIT
};


class FileWatcher
    :
    #ifdef USING_WINDOWS
        public FileWatcherBaseWindows
    #elif defined(USING_LINUX)
    #elif defined(USING_LINUX)
    #endif // defined
{

    std::string currentArgument;
    std::string currentDirectory;
    unsigned pollingFrequency;
    static unsigned long long lastUpdatedTime;

public:

    bool closeProgram = false;

    FileWatcher(std::string);
    FileWatcher(){};
    ~FileWatcher();

    void poll();
    void close();
    void print(const char*, std::initializer_list<Colors>);
    void print(std::string, std::initializer_list<Colors>);
    std::string getCurrentWatchedDirectory();
    std::string getCurrentSetDirectory();

    void queryForDirectoryName();
    void displayDirectoryTree(std::string, bool);
    Commands parseCommand(std::string);

    void setPollingRateMilliseconds(unsigned);
};

#endif // H_FILE_WATCHER_H
