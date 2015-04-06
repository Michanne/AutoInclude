#ifndef H_FILE_WATCHER_BASE_H
#define H_FILE_WATCHER_BASE_H

#define PURE_VIRTUAL        =0

class FileWatcherBase
{

public:
    FileWatcherBase();
    virtual ~FileWatcherBase();

    virtual void watchFile()            PURE_VIRTUAL;
    virtual void watchHeader()          PURE_VIRTUAL;
    virtual void autoInclude()          PURE_VIRTUAL;
    virtual void traverseDirectory()    PURE_VIRTUAL;

    virtual void setPollFrequency()     PURE_VIRTUAL;
};

#endif // H_FILE_WATCHER_BASE_H
