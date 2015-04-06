#ifndef H_FILE_WATCHER_BASE_H
#define H_FILE_WATCHER_BASE_H

#define PURE_VIRTUAL        =0

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <initializer_list>

typedef enum FileWatcherColorBase
{
    RED,
    GREEN,
    BLUE,
    INTENSE,
    B_RED,
    B_GREEN,
    B_BLUE,
    B_INTENSE
} Colors;

enum FileState
{
    REMOVED,
    MODIFIED,
    ADDED,
    UNCHANGED
};

struct FileWatcherConfig
{

    std::string                                   currentlyWatchedDirectory;
    std::unordered_map<std::string, FileState>    currentlyWatchedFiles;
};

class FileWatcherBase
{

public:
    FileWatcherBase(){};
    virtual ~FileWatcherBase(){};

    virtual void setWatchedDirectory(std::string)   PURE_VIRTUAL;

    virtual void watchFile(std::string)             PURE_VIRTUAL;
    virtual void watchHeader(std::string)           PURE_VIRTUAL;
    virtual void autoInclude()                      PURE_VIRTUAL;
    virtual void traverseDirectory()                PURE_VIRTUAL;

    virtual void platformWaitThread()               PURE_VIRTUAL;
    virtual void platformHoldForQuit()              PURE_VIRTUAL;
    virtual void platformPrintColorC(const char*, std::initializer_list<Colors>)    PURE_VIRTUAL;
    virtual void platformPrintColorS(std::string, std::initializer_list<Colors>)    PURE_VIRTUAL;
    virtual std::string platformQueryDirectory()    PURE_VIRTUAL;

    virtual void beginDirectoryWatch()              PURE_VIRTUAL;

    void setPollFrequency();

    bool log(const char*, unsigned);
};

#endif // H_FILE_WATCHER_BASE_H
