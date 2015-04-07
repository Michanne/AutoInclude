#include <iostream>
#define USING_WINDOWS
#include "FileWatcher.h"

//TODO:
/**
need to finish header generation and maybe add support for watching assets too?
watching works, although not as it should
polling blocks main thread right now. We'll fix this when I add support for Rebuild and Rebase options
**/

FileWatcher* startup();
void shutdown(FileWatcher*);
void run(FileWatcher*);

FileWatcher* watcher;

int main(int argc, char** argv)
{
/*
    if(argc == 1)
    {
        watcher = new FileWatcher(argv[0]);
    }*/

    watcher = startup();

    run(watcher);

    shutdown(watcher);
    return 0;
}

FileWatcher* startup()
{

    FileWatcher* fw = new FileWatcher();
    return fw;
}

void shutdown(FileWatcher* fw)
{

    fw->close();
    delete fw;
}

void run(FileWatcher* fw)
{

    fw->print("Hello World\n", {Colors::RED});
    fw->queryForDirectoryName();
    fw->setPollingRateMilliseconds(5000);
    if(!fw->closeProgram)
    {
        fw->setWatchedDirectory(fw->getCurrentSetDirectory());
        fw->platformBeginDirectoryWatch();
    }
    while(!fw->closeProgram)
    {
        fw->poll();
    }
}
