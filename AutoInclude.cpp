#include "AutoInclude.h"

AutoInclude::AutoInclude(std::string directoryName)
{
}

AutoInclude::~AutoInclude()
{

}

void AutoInclude::close(std::string directory)
{
    watcher->stopWatchInstance(NULL);
}

std::string AutoInclude::getCurrentSetDirectory()
{
    return currentDirectory;
}

void AutoInclude::queryForDirectoryName()
{
    currentDirectory = BASE_PATH;
    showDirectory(currentDirectory, true);
    //once directory is chosen, add a new watch instance on the directory
}

Commands AutoInclude::parseCommand(std::string line)
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
            watcher->print("Usage [" + arg1 + "]:\t" + CommandUsages.at(arg1), {Colors::RED});
            std::cout << "\n";
            option = Commands::INVALID;
        }
    }

    else if(arg1 == "help")
    {
        for(auto comm : CommandUsages)
        {
            watcher->print("Usage [" + comm.first + "]:\t" + comm.second, {Colors::WHITE});
            std::cout << "\n";
            option = Commands::HELP;
        }
    }

    else
    {
        watcher->print("'" + line + "' is an invalid command", {Colors::BLUE, Colors::GREEN});
        std::cout << std::endl;
        option = Commands::INVALID;
        currentArguments = arguments;
    }

    return option;
}

#define CASE(x, y) case x: y break;

void AutoInclude::showDirectory(std::string directory, bool showTree)
{

    //display the tree
    if(showTree)
    if(watcher->displayDirectoryTree(directory))
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
         if(!watcher->displayDirectoryTree(currentDirectory + "/" + currentArguments.at(1)))
            showDirectory(currentDirectory, true);
         else currentDirectory += "/" + currentArguments.at(1);
         );

    CASE(CHANGE_RELATIVE_DIRECTORY,
         showDirectory(currentDirectory + "/" + currentArguments.at(1), true););

    CASE(CHANGE_ABSOLUTE_DIRECTORY,
         showDirectory(currentArguments.at(1), true););

    CASE(INVALID,
         std::cout << "Please enter a valid command" << std::endl;
         showDirectory(currentDirectory, false););

    CASE(PRINT_CURRENT_DIRECTORY,
         showDirectory(currentDirectory, true););

    CASE(HELP,
         showDirectory(currentDirectory, false););

    CASE(QUIT, closeProgram = true;);
    }
}

#undef CASE

void AutoInclude::setPollingRateMilliseconds(unsigned pollRate)
{
    pollingFrequency = pollRate;
}

void AutoInclude::poll()
{

    //watcher->platformThreadedWait(pollingFrequency);
    //pollWatcherThread();
}

