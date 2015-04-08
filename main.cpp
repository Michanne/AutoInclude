#include <iostream>
#define USING_WINDOWS
#include "AutoInclude.h"

//TODO:
/**
need to finish header generation and maybe add support for watching assets too?
watching works, although not as it should
polling blocks main thread right now. We'll fix this when I add support for Rebuild and Rebase options
**/

AutoInclude* startup();
void shutdown(AutoInclude*);
void run(AutoInclude*);

AutoInclude* application;

int main(int argc, char** argv)
{
    application = startup();

    run(application);

    shutdown(application);
    return 0;
}

AutoInclude* startup()
{

    AutoInclude* ai = new AutoInclude();
    return ai;
}

void shutdown(AutoInclude* ai)
{

    ai->close();
    delete ai;
}

void run(AutoInclude* ai)
{

    ai->print("Hello World\n", {Colors::RED});
    ai->queryForDirectoryName();
    ai->setPollingRateMilliseconds(5000);
    if(!ai->closeProgram)
    {
        ai->setWatchedDirectory(fw->getCurrentSetDirectory());
        ai->platformBeginDirectoryWatch();
    }
    while(!fw->closeProgram)
    {
        ai->poll();
    }
}
