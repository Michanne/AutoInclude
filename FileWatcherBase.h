#ifndef H_FILE_WATCHER_BASE_H
#define H_FILE_WATCHER_BASE_H

#define PURE_VIRTUAL        =0
#define DELETED             =delete
#define IMPLEMENTED

#define WATCH_FILE_NAME_CHANGE      0x1
#define WATCH_FILE_CREATE           0x2
#define WATCH_FILE_REMOVE           0x3

#define COLOR_OPTIONS   10
#define FILE_HANDLE_STATES  5

#define FILE_WATCHER_ERROR_THREAD_FAIL_SPAWN                        "Worker thread could not be spawned"
#define FILE_WATCHER_ERROR_WATCHPOINT_FAIL_SPAWN(directory)         "Unable to create watchpoint in directory '" + directory + "'"
#define FILE_WATCHER_ERROR_NOTIFY_INVALID                           "Notification handle was invalid"
#define FILE_WATCHER_ERROR_NOTIFY_FAIL(directory)                   "Unable to request change in directory '" + directory + "'"

#define FILE_WATCHER_WARN_NOTIFY_UPDATE_FAIL(attempts)              "Unable to request additional change notification: Attempt #" + attempts
#define FILE_WATCHER_WARN_DIR_EXIST(directory)                      "The directory has already been created at '" + directory + "'"
#define FILE_WATCHER_WARN_DIR_FAIL_DISPLAY(directory)               "'" + directory + "' was unable to be displayed"

#define FILE_WATCHER_COLOR_DIR_FAIL_DISPLAY                         {Colors::RED, Colors::INTENSE, Colors::B_RED}
#define FILE_WATCHER_COLOR_DIR_WATCH                                {Colors::BLUE, Colors::GREEN}
#define FILE_WATCHER_COLOR_FILE_HEADER                              {Colors::BLUE, Colors::GREEN}
#define FILE_WATCHER_COLOR_FILE_IMPLEMENTATION                      {Colors::GREEN}
#define FILE_WATCHER_COLOR_FILE_FOLDER                              {Colors::BLUE}
#define FILE_WATCHER_COLOR_FILE_FILE                                {Colors::RED, Colors::BLUE}

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <initializer_list>
#include <queue>
#include <functional>

typedef unsigned int WatchFileOption;

typedef enum FileWatcherColorBase
{
    RED,
    GREEN,
    BLUE,
    INTENSE,
    WHITE,
    B_RED,
    B_GREEN,
    B_BLUE,
    B_INTENSE,
    B_WHITE
} Colors;

enum FileHandleState
{
    REMOVED,
    MODIFIED,
    ADDED,
    RENAMED_OLD,
    RENAMED_NEW,
};

enum FileState
{
    UNCHANGED,
    CHANGED
};

enum FileType
{
    ftFILE,
    ftFOLDER,
    ftHEADER,
    ftIMPLEMENTATION
};

struct FileInfo
{
    std::string absoluteFilename;
    std::string shorthandName;
};

struct FileWatcherConfig
{

    std::string                                     currentlyWatchedDirectory;
    std::unordered_map<std::string, FileState>      currentlyWatchedFiles_States;
    std::unordered_map<std::string, FileHandleState>currentlyWatchedFiles_Handles;
    unsigned                                        pollingRate;
};

struct WatchEventInformation
{
    std::string relativeFilename;
    std::string directoryName;
    WatchFileOption currentOptions;
    std::function<void(WatchFileOption)> function;
};

#define NULL_HANDLE                 0

#define WATCH_HANDLE                unsigned int

typedef WATCH_HANDLE WatchInstanceHandle;
typedef WATCH_HANDLE WatchEventHandle;

struct WatchHandleAllocator
{
    #define WATCH_HANDLE_ALLOCATOR_MAX_SIZE 1024
    bool usedIndices[WATCH_HANDLE_ALLOCATOR_MAX_SIZE];
    std::queue<WATCH_HANDLE> freeIndices;
    unsigned size;

    WATCH_HANDLE getHandle();
    bool okay(WATCH_HANDLE);
    void remove(WATCH_HANDLE);
};

class FileWatcherBase
{

protected:

    #define _pair(x, y) {x, FileType::ft##y}
    std::unordered_map<std::string, FileType> FileExtensions =
    {
        _pair(".h", HEADER), _pair(".hpp", HEADER), _pair(".c", IMPLEMENTATION),
        _pair(".cpp", IMPLEMENTATION), _pair(".tcc", IMPLEMENTATION), _pair("\0", FOLDER)
    };
    #undef _pair

    std::unordered_map<std::string, std::string> FileShortHandNames;
    std::vector<WatchEventInformation*> watchDelegates;
    WatchHandleAllocator allocEvent;

    static bool fileChanged;

public:
    FileWatcherBase(){};
    FileWatcherBase(FileWatcherBase& rhs)                                           DELETED;
    virtual ~FileWatcherBase(){};

    virtual void setWatchedDirectory(std::string)                                   PURE_VIRTUAL;

    virtual void watchDirectory()                                                   PURE_VIRTUAL;

    virtual void platformCloseThread()                                              PURE_VIRTUAL;
    virtual void platformPrintColorC(const char*, std::initializer_list<Colors>)    PURE_VIRTUAL;
    virtual void platformPrintColorS(std::string, std::initializer_list<Colors>)    PURE_VIRTUAL;
    virtual std::string platformQueryDirectory()                                    PURE_VIRTUAL;
    virtual bool platformDisplayDirectory(std::string)                              PURE_VIRTUAL;
    virtual FileType platformFileType(std::string)                                  PURE_VIRTUAL;
    virtual void platformBeginDirectoryWatch()                                      PURE_VIRTUAL;
    virtual void platformCreateDirectory(std::string)                               PURE_VIRTUAL;

    virtual void pollWatcherThread()                                                PURE_VIRTUAL;

    virtual void platformThreadedWait(unsigned)                                     PURE_VIRTUAL;

    void removeEvent(WatchEventHandle)                                              IMPLEMENTED;

    void watchFile(std::string)                                                     IMPLEMENTED;
    void watchHeader(std::string)                                                   IMPLEMENTED;
    void autoInclude()                                                              IMPLEMENTED;
    void traverseDirectory()                                                        IMPLEMENTED;

    std::string queryForShorthandName(std::string originalFilename)                 IMPLEMENTED;
    void handleFileState(FileHandleState fhs, std::string filename)                 IMPLEMENTED;

    bool log(const char*, unsigned);

};

#endif // H_FILE_WATCHER_BASE_H
