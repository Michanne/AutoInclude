#include "FileWatcherWindows.h"

bool FileWatcherBaseWindows::M_threadDead = false;
FileWatcherConfig FileWatcherBaseWindows::M_config;
HANDLE FileWatcherBaseWindows::M_hDirectoryStatus;

FileWatcherBaseWindows::FileWatcherBaseWindows(std::string directoryName)
{
    setWatchedDirectory(directoryName);
}

FileWatcherBaseWindows::FileWatcherBaseWindows()
{
}

FileWatcherBaseWindows::~FileWatcherBaseWindows()
{
    platformWaitThread();
}

void FileWatcherBaseWindows::setWatchedDirectory(std::string directoryName)
{
    M_config.currentlyWatchedDirectory = directoryName;
}

void FileWatcherBaseWindows::watchFile(std::string filename)
{

}

void FileWatcherBaseWindows::watchHeader(std::string filename)
{

}

void FileWatcherBaseWindows::autoInclude()
{

}

void FileWatcherBaseWindows::traverseDirectory()
{

}

void FileWatcherBaseWindows::platformWaitThread()
{

    M_threadDead = true;
    WaitForSingleObject(m_hWatcherThread, INFINITE);
}

void FileWatcherBaseWindows::platformHoldForQuit()
{

    std::string quitState;
    while(std::cin >> quitState)
        if(quitState == "quit"  ||
           quitState == "exit"  ||
           quitState == "done"  ||
           quitState == "q"     ) break;
}

void FileWatcherBaseWindows::platformPrintColorS(std::string text, std::initializer_list<Colors> fwcb)
{
    platformPrintColorC(text.c_str(), fwcb);
}

void FileWatcherBaseWindows::platformPrintColorC(const char* text, std::initializer_list<Colors> fwcb)
{

    static HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hStdOut, &csbi);

    WORD option = 0;

    for(Colors color : fwcb)
    {
        option |= wTerminalColors[color % COLOR_OPTIONS];
    }

    SetConsoleTextAttribute(hStdOut, option);

    std::cout << text;

    SetConsoleTextAttribute(hStdOut, csbi.wAttributes);
}

std::string FileWatcherBaseWindows::platformQueryDirectory()
{
    return M_config.currentlyWatchedDirectory;
}

void FileWatcherBaseWindows::beginDirectoryWatch()
{

    //spawn worker thread
    m_hWatcherThread = CreateThread(
                                  NULL,
                                  0,
                                  watchDirectory,
                                  NULL,
                                  0,
                                  &m_dwThreadId);

    //check if worker thread spawn successfully
    if(m_hWatcherThread == NULL)
    {

        //create log file and log the error, close program
        log("Worker thread could not be spawned. Exiting with error code 1", 200);
        ExitProcess(EXIT_FAILURE);
    }
}

DWORD WINAPI FileWatcherBaseWindows::watchDirectory(LPVOID lpParam)
{

    if(M_hDirectoryStatus == INVALID_HANDLE_VALUE)
    {
        log("Unable to create watchpoint in directory '" + M_config.currentlyWatchedDirectory + "'", 200);
        ExitProcess(EXIT_FAILURE);
    }

    while(!M_threadDead)
    {

        //query a change in the current directory
        M_hDirectoryStatus = FindFirstChangeNotification(
                                    M_config.currentlyWatchedDirectory.c_str(),
                                    true,
                                    FILE_NOTIFY_CHANGE_FILE_NAME
                                    );

    }

    return 0;
}

bool FileWatcherBaseWindows::log(std::string text, unsigned bufferSize)
{
    return log(text.c_str(), bufferSize);
}

bool FileWatcherBaseWindows::log(const char* text, unsigned bufferSize)
{

    std::ofstream out_file("logfile.txt", std::ofstream::app);

    SYSTEMTIME stCurrentDate;
    GetLocalTime(&stCurrentDate);

    char cBuffer[bufferSize];

    snprintf(cBuffer, bufferSize, "%d-%d-%d[%d:%d %s]:\t%s",
         stCurrentDate.wMonth,
         stCurrentDate.wDay,
         stCurrentDate.wYear,
         stCurrentDate.wHour % 24,
         stCurrentDate.wMinute,
         (stCurrentDate.wHour > 12 ? "PM" : "AM"),
         text);

    out_file << cBuffer << std::endl;
    out_file.close();

    return true;
}
