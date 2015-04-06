#ifndef H_FILE_WATCHER_WINDOWS_BASE_H
#define H_FILE_WATCHER_WINDOWS_BASE_H
#pragma once

#include "../FileWatcherBase.h"
#include <windows.h>

#define COLOR_OPTIONS   8

class FileWatcherBaseWindows : public FileWatcherBase
{

private:

    WORD wTerminalColors[COLOR_OPTIONS] =
    {
        FOREGROUND_RED, FOREGROUND_GREEN, FOREGROUND_BLUE, FOREGROUND_INTENSITY,
        BACKGROUND_RED, BACKGROUND_GREEN, BACKGROUND_BLUE, BACKGROUND_INTENSITY
    };

    HANDLE                      m_hWatcherThread;
    DWORD                       m_dwThreadId;
    static bool                 M_threadDead;
    static FileWatcherConfig    M_config;
    static HANDLE               M_hDirectoryStatus;

    const char* generateMainHeader();
    const char* generateHeaderFromURL();
    const char* generateHeaderFromImplementation();

    static DWORD WINAPI   watchDirectory(LPVOID);

public:

    FileWatcherBaseWindows(std::string);
    FileWatcherBaseWindows();
    ~FileWatcherBaseWindows();

    void setWatchedDirectory(std::string);

    void watchFile(std::string);
    void watchHeader(std::string);
    void autoInclude();
    void traverseDirectory();

    void platformWaitThread();
    void platformHoldForQuit();
    void platformPrintColorC(const char*, std::initializer_list<Colors>);
    void platformPrintColorS(std::string, std::initializer_list<Colors>);
    std::string platformQueryDirectory();

    void beginDirectoryWatch();

    static bool log(std::string, unsigned);
    static bool log(const char*, unsigned);
};

#endif // H_FILE_WATCHER_WINDOWS_BASE_H
