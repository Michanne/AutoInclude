#ifndef H_FILE_WATCHER_BASE_H
#define H_FILE_WATCHER_BASE_H

#define PURE_VIRTUAL        =0
#define DELETED             =delete
#define IMPLEMENTED

#define FILE_HANDLE_STATES  5
#define AUTO_INCLUDE_BASE_PATH  "autoinclude"

#define AUTO_INCLUDE_ERROR_THREAD_FAIL_SPAWN                        "Worker thread could not be spawned"
#define AUTO_INCLUDE_ERROR_WATCHPOINT_FAIL_SPAWN(directory)         "Unable to create watchpoint in directory '" + directory + "'"
#define AUTO_INCLUDE_ERROR_NOTIFY_INVALID                           "Notification handle was invalid"
#define AUTO_INCLUDE_ERROR_NOTIFY_FAIL(directory)                   "Unable to request change in directory '" + directory + "'"

#define AUTO_INCLUDE_WARN_NOTIFY_UPDATE_FAIL(attempts)              "Unable to request additional change notification: Attempt #" + attempts
#define AUTO_INCLUDE_WARN_AI_DIR_EXIST(directory)                   "The auto-include directory has already been created at '" + directory + "'"
#define AUTO_INCLUDE_WARN_DIR_FAIL_DISPLAY(directory)               "'" + directory + "' was unable to be displayed"

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

class FileWatcherBase
{

protected:

    #define _pair(x, y) {x, FileType::ft##y}
    std::unordered_map<std::string, FileType> FileExtensions =
    {
        _pair("h", HEADER), _pair("hpp", HEADER), _pair("c", IMPLEMENTATION),
        _pair("cpp", IMPLEMENTATION), _pair("tcc", IMPLEMENTATION), _pair("\0", FOLDER)
    };
    #undef _pair

    std::unordered_map<std::string, std::string> FileShortHandNames;

    static bool fileChanged;

public:
    FileWatcherBase(){};
    FileWatcherBase(FileWatcherBase& rhs)                                           DELETED;
    virtual ~FileWatcherBase(){};

    virtual void setWatchedDirectory(std::string)                                   PURE_VIRTUAL;

    virtual void watchDirectory()                                                   PURE_VIRTUAL;

    virtual void platformWaitThread()                                               PURE_VIRTUAL;
    virtual void platformPrintColorC(const char*, std::initializer_list<Colors>)    PURE_VIRTUAL;
    virtual void platformPrintColorS(std::string, std::initializer_list<Colors>)    PURE_VIRTUAL;
    virtual std::string platformQueryDirectory()                                    PURE_VIRTUAL;
    virtual bool platformDisplayDirectory(std::string)                              PURE_VIRTUAL;
    virtual FileType platformFileType(std::string)                                  PURE_VIRTUAL;
    virtual void platformBeginDirectoryWatch()                                      PURE_VIRTUAL;
    virtual void platformReadyAIDirectory()                                         PURE_VIRTUAL;
    virtual void platformGenerateHeaderFile()                                       PURE_VIRTUAL;
    virtual void platformOpenShorthandList()                                        PURE_VIRTUAL;

    virtual void generateShorthandList()                                            PURE_VIRTUAL;
    virtual void parseShorthandList()                                               PURE_VIRTUAL;

    virtual void pollWatcherThread()                                                PURE_VIRTUAL;

    virtual void platformThreadedWait(unsigned)                                     PURE_VIRTUAL;

    void watchFile(std::string)                                                     IMPLEMENTED;
    void watchHeader(std::string)                                                   IMPLEMENTED;
    void autoInclude()                                                              IMPLEMENTED;
    void traverseDirectory()                                                        IMPLEMENTED;

    std::string queryForShorthandName(std::string originalFilename)                 IMPLEMENTED;
    void handleFileState(FileHandleState fhs, std::string filename)                 IMPLEMENTED;

    bool log(const char*, unsigned);

};

#endif // H_FILE_WATCHER_BASE_H
