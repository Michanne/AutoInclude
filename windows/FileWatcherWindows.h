#ifndef H_FILE_WATCHER_WINDOWS_BASE_H
#define H_FILE_WATCHER_WINDOWS_BASE_H
#pragma once

#define UNICODE
#include "../FileWatcherBase.h"
#include <vector>
#include <boost/locale/encoding_utf.hpp> //yes "boost", I know :^)
#include <locale>
#include <shlwapi.h> //link using -lshlwapi
#include <windows.h>

#define _MAX_PATH       1000
#define BASE_PATH "C:"

using boost::locale::conv::utf_to_utf;

class WatchInstance : public FileWatcherBase
{

private:

    static WatchInstance* instance;

    WORD wTerminalColors[COLOR_OPTIONS] =
    {
        FOREGROUND_RED, FOREGROUND_GREEN, FOREGROUND_BLUE, FOREGROUND_INTENSITY, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED,
        BACKGROUND_RED, BACKGROUND_GREEN, BACKGROUND_BLUE, BACKGROUND_INTENSITY, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED
    };

    DWORD FileHandleStateToDWORDFlag[FILE_HANDLE_STATES] =
    {
        FILE_ACTION_REMOVED, FILE_ACTION_MODIFIED, FILE_ACTION_ADDED,
        FILE_ACTION_RENAMED_OLD_NAME, FILE_ACTION_RENAMED_NEW_NAME
    };

    HANDLE                      m_hWatcherThread;
    DWORD                       m_dwThreadId;
    bool                        m_threadDead;
    FileWatcherConfig           m_config;
    HANDLE                      m_hDirectoryStatus;

    const char* generateMainHeader();
    const char* generateHeaderFromURL();
    const char* generateHeaderFromImplementation();

    static DWORD WINAPI   watchDirectoryEntryPoint(LPVOID);

public:

    WatchInstance(std::string);
    WatchInstance();
    ~WatchInstance();

    void setWatchedDirectory(std::string);
    void watchDirectory();

    void platformCloseThread();
    void platformPrintColorC(const char*, std::initializer_list<Colors>);
    void platformPrintColorS(std::string, std::initializer_list<Colors>);
    std::string platformQueryDirectory();
    bool platformDisplayDirectory(std::string);
    FileType platformFileType(std::string);
    void platformThreadedWait(unsigned);
    void platformSetCurrentDirectory(std::string);

    void platformCreateDirectory(std::string);

    void pollWatcherThread();

    void beginDirectoryWatch();

    static bool log(std::string, unsigned);
    static bool log(const char*, unsigned);

    template <typename FUNCTION>
    void platformCreateThread(FUNCTION);

    //http://stackoverflow.com/questions/15615136/is-codecvt-not-a-std-header
    //provided by: jotrocken

    static std::wstring utf8_to_wstring(const std::string& str)
    {
        return utf_to_utf<wchar_t>(str.c_str(), str.c_str() + str.size());
    }

    static std::string wstring_to_utf8(wchar_t* str2, unsigned bufferSize)
    {
        std::wstring str(str2);
        return utf_to_utf<char>(str.c_str(), str.c_str() + bufferSize);
    }

    static std::string wstring_to_utf8(const std::wstring& str)
    {
        return utf_to_utf<char>(str.c_str(), str.c_str() + str.size());
    }
};

#include "FileWatcherWindows.tcc"
#endif // H_FILE_WATCHER_WINDOWS_BASE_H
