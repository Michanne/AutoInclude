#include "FileWatcher.h"

unsigned long long FileWatcher::lastUpdatedTime;

FileWatcher::FileWatcher(std::string directoryName)
:
    #ifdef USING_WINDOWS
    FileWatcherBaseWindows(directoryName.c_str())
    #endif //constructor
{

    print("Watching directory " + directoryName + "...\n",
          {Colors::RED, Colors::GREEN});

    lastUpdatedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

FileWatcher::~FileWatcher()
{

}

void FileWatcher::close()
{
    platformWaitThread();
}

void FileWatcher::print(std::string text, std::initializer_list<Colors> colors)
{
    platformPrintColorS(text, colors);
}

void FileWatcher::print(const char* text, std::initializer_list<Colors> colors)
{
    platformPrintColorS(text, colors);
}

std::string FileWatcher::getCurrentWatchedDirectory()
{
    return platformQueryDirectory();
}

std::string FileWatcher::getCurrentSetDirectory()
{
    return currentDirectory;
}

void FileWatcher::queryForDirectoryName()
{
    currentDirectory = BASE_PATH;
    displayDirectoryTree(currentDirectory, true);
}

#define PRINT_USAGE(x) std::cout << "Usage: " << x << std::endl; return Commands::INVALID;

Commands FileWatcher::parseCommand(std::string line)
{
    std::vector<std::string> arguments;
    std::istringstream line_ss(line);
    std::string token;
    Commands option;

    while(line_ss >> token) arguments.push_back(token);

    std::string arg1 = arguments.at(0);

    if(CommandNames.find(arg1) != CommandNames.end())
    {
        if(arguments.size() == CommandArgs.at(arg1))
        {
            currentArguments = arguments;
            option = CommandNames.at(arg1);
        }
        else
        {
            print("Usage [" + arg1 + "]:\t" + CommandUsages.at(arg1), {Colors::RED});
            std::cout << "\n";
            option = Commands::INVALID;
        }
    }

    else
    {
        print("'" + line + "' is an invalid command", {Colors::BLUE, Colors::GREEN});
        std::cout << std::endl;
        option = Commands::INVALID;
        currentArguments = arguments;
    }

    return option;
}

#define CASE(x, y) case x: y break;

void FileWatcher::displayDirectoryTree(std::string directory, bool showTree)
{

    //display the tree
    if(showTree)
        if(platformDisplayDirectory(directory + "/*"))
        {
            std::cout << "Select the directory using the directory tree above\nAlternatively, type the absolute path of the directory\n"
                         "Type 'help' for help:"<< std::endl;
            currentDirectory = directory;
        }

    //wait for the user to enter a command
    std::string commandLine;
    std::cout << currentDirectory << "> ";
    std::getline(std::cin, commandLine);
    Commands command = parseCommand(commandLine);

    switch(command)
    {
    CASE(SELECT,
         if(!platformDisplayDirectory(currentDirectory + "/" + currentArguments.at(1)))
            displayDirectoryTree(currentDirectory, true);
         else currentDirectory += "/" + currentArguments.at(1);
         );

    CASE(CHANGE_RELATIVE_DIRECTORY,
         displayDirectoryTree(currentDirectory + "/" + currentArguments.at(1), true););

    CASE(CHANGE_ABSOLUTE_DIRECTORY,
         displayDirectoryTree(currentArguments.at(1), true););

    CASE(INVALID,
         std::cout << "Please enter a valid command" << std::endl;
         displayDirectoryTree(currentDirectory, false););

    CASE(QUIT, closeProgram = true;);
    }
}

#undef CASE

void FileWatcher::setPollingRateMilliseconds(unsigned pollRate)
{
    pollingFrequency = pollRate;
}

void FileWatcher::poll()
{

    platformThreadedWait(pollingFrequency);
    pollWatcherThread();
}
