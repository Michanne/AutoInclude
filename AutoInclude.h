#ifndef H_AUTO_INCLUDE_H
#define H_AUTO_INCLUDE_H
#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include "FileWatcher.h"

enum Commands
{
    INVALID,
    HELP,
    SELECT,
    VIEW,
    PRINT_CURRENT_DIRECTORY,
    CHANGE_ABSOLUTE_DIRECTORY,
    CHANGE_RELATIVE_DIRECTORY,
    REBASE,
    REBUILD,
    QUIT
};

class AutoInclude
{

private:

    #define _p(x, y) {x, y},
    std::unordered_map<std::string, std::string> CommandUsages =
    {
        _p("cd", "<path-name> ; Switch to <path-name> directory")
        _p("cda", "<path-name-absolute> ; Switch to <path-name-absolute> directory")
        _p("pwd", "; Prints the file structure of the current directory")
        _p("quit", "; Quits the program")
        _p("select", "<path-name> ; Selects <path-name> in current directory to watch")
        _p("rebase", "")
        _p("rebuild", "")
    };

    #define c(x) Command:x
    std::unordered_map<std::string, Commands> CommandNames =
    {
        _p("cd", c(CHANGE_RELATIVE_DIRECTORY))
        _p("cda", c(CHANGE_ABSOLUTE_DIRECTORY))
        _p("pwd", c(PRINT_CURRENT_DIRECTORY))
        _p("quit", c(QUIT))
        _p("select", c(SELECT))
        _p("rebase", c(REBASE))
        _p("rebuild", c(REBUILD))
    };
    #undef c

    std::unordered_map<std::string, unsigned> CommandArgs =
    {
        _p("cd", 2)
        _p("cda", 2)
        _p("pwd", 1)
        _p("quit", 1)
        _p("select", 2)
        _p("rebase", 3)
        _p("rebuild", 1)
    };
    #undef _p

    std::vector<std::string> currentArguments;
    std::string currentDirectory;
    unsigned pollingFrequency;
    static unsigned long long lastUpdatedTime;
    FileWatcher::FileWatcher* watcher;

public:

    bool closeProgram = false;

    AutoInclude(std::string);
    AutoInclude(){};
    ~AutoInclude();

    void poll();
    void close(std::string);
    void showDirectory(std::string, bool);
    std::string getCurrentSetDirectory();

    void queryForDirectoryName();
    Commands parseCommand(std::string);

    void setPollingRateMilliseconds(unsigned);
};

#endif // H_FILE_WATCHER_H

