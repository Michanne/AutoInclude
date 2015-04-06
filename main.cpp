#include <iostream>
#define USING_WINDOWS
#include "FileWatcher.h"

enum Commands
{
    SELECT,
    VIEW,
    PRINT_CURRENT_DIRECTORY
};

FileWatcher* startup();
void shutdown(FileWatcher*);
void run(FileWatcher*);

std::string queryForDirectoryName();
void displayDirectoryTree(std::string);
void parseCommand(std::string);

int main(int argc, char** argv)
{

    FileWatcher* watcher;

    if(argc == 1)
    {
        watcher = new FileWatcher(argv[0]);
    }

    else watcher = startup();

    run(watcher);

    shutdown(watcher);
    return 0;
}

FileWatcher* startup()
{

    FileWatcher* fw = new FileWatcher(queryForDirectoryName());
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
    displayDirectoryTree(fw->getCurrentDirectory());
    fw->wait();
}

std::string queryForDirectoryName()
{
    std::cout << "Select the directory using the directory tree below\nAlternatively, type the absolute path of the directory:" << std::endl;
    displayDirectoryTree("C:\\");
}

void displayDirectoryTree(std::string directory)
{

}
