#ifndef H_FILE_WATCHER_BASE_H
#define H_FILE_WATCHER_BASE_H

#define PURE_VIRTUAL        =0
#define FILE_HANDLE_STATES  5

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
    bool go;

public:
    FileWatcherBase(){};
    virtual ~FileWatcherBase(){};

    virtual void setWatchedDirectory(std::string)   PURE_VIRTUAL;

    virtual void watchDirectory()                   PURE_VIRTUAL;

    virtual void watchFile(std::string)             PURE_VIRTUAL;
    virtual void watchHeader(std::string)           PURE_VIRTUAL;
    virtual void autoInclude()                      PURE_VIRTUAL;
    virtual void traverseDirectory()                PURE_VIRTUAL;

    virtual void platformWaitThread()               PURE_VIRTUAL;
    virtual void platformPrintColorC(const char*, std::initializer_list<Colors>)    PURE_VIRTUAL;
    virtual void platformPrintColorS(std::string, std::initializer_list<Colors>)    PURE_VIRTUAL;
    virtual std::string platformQueryDirectory()    PURE_VIRTUAL;
    virtual bool platformDisplayDirectory(std::string)                              PURE_VIRTUAL;
    virtual FileType platformFileType(std::string)  PURE_VIRTUAL;
    virtual void platformBeginDirectoryWatch()      PURE_VIRTUAL;

    virtual void generateShorthandList()            PURE_VIRTUAL;
    virtual void openShorthandList()                PURE_VIRTUAL;
    virtual void parseShorthandList()               PURE_VIRTUAL;
    virtual void generateHeaderFile()               PURE_VIRTUAL;

    virtual void pollWatcherThread()                PURE_VIRTUAL;

    virtual void platformThreadedWait(unsigned)     PURE_VIRTUAL;

    bool log(const char*, unsigned);

    std::string queryForShorthandName(std::string originalFilename)
    {
        std::string temp;
        platformPrintColorS("What name would you like to give to '" + originalFilename + "'?", {Colors::BLUE, Colors::GREEN, Colors::B_BLUE, Colors::B_INTENSE});
        std::cout << std::endl;
        std::getline(std::cin, temp);
        return temp;
    }

    void handleFileState(FileHandleState fhs, std::string filename)
    {
        char* yesno;

        switch(fhs)
        {
        case FileHandleState::ADDED:
            platformPrintColorS(filename + " was created in the directory. Would you like to add the file to this autoinclude project?",
                                {Colors::GREEN});
            std::cin.get(yesno, 1);
            if(yesno[0] == 'y')
                queryForShorthandName(filename);
                break;
        }
    }
};

#endif // H_FILE_WATCHER_BASE_H
