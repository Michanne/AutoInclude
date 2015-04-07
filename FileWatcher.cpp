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

    switch(line.c_str()[0])
    {
    case 'c':
        if(arg1 == "cd")
        {

            if(arguments.size() != 2)
            {
                PRINT_USAGE("cd <path-name> ; Switch to <path-name> directory");
            }

            currentArgument = arguments.at(1);
            option = Commands::CHANGE_RELATIVE_DIRECTORY;
        }
        if(arg1 == "cda")
        {

            if(arguments.size() != 2)
            {
                PRINT_USAGE("cda <path-name-absolute> ; Switch to <path-name-absolute> directory");
            }

            currentArgument = arguments.at(1);
            option = Commands::CHANGE_ABSOLUTE_DIRECTORY;
        }
        break;
    case 'q':
        if(arg1 == "quit" ||
           arg1 == "exit" ||
           arg1 == "q")
        {
            currentArgument = "";
            option = Commands::QUIT;
        }
        break;
    case 's':
        if(arg1 == "select")
        {

            if(arguments.size() != 2)
            {
                PRINT_USAGE("select <path-name> ; Selects <path-name> in current directory to watch");
            }
            currentArgument = arguments.at(1);
            option = Commands::SELECT;
        }
        break;
    default:
        print("'" + line + "' is an invalid command", {Colors::BLUE, Colors::GREEN});
        std::cout << std::endl;
        option = Commands::INVALID;
        currentArgument = "invalid";
        break;
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
         if(!platformDisplayDirectory(currentDirectory + "/" + currentArgument))
            displayDirectoryTree(currentDirectory, true);
         else currentDirectory += "/" + currentArgument;
         );

    CASE(CHANGE_RELATIVE_DIRECTORY,
         displayDirectoryTree(currentDirectory + "/" + currentArgument, true););

    CASE(CHANGE_ABSOLUTE_DIRECTORY,
         displayDirectoryTree(currentArgument, true););

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
