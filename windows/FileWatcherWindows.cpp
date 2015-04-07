#include "FileWatcherWindows.h"

bool FileWatcherBaseWindows::M_threadDead = false;
FileWatcherConfig FileWatcherBaseWindows::M_config;
HANDLE FileWatcherBaseWindows::M_hDirectoryStatus;
FileWatcherBaseWindows* FileWatcherBaseWindows::instance = nullptr;



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

DWORD WINAPI FileWatcherBaseWindows::watchDirectoryEntryPoint(LPVOID lpParam)
{
    static_cast<FileWatcherBase*>(lpParam)->watchDirectory();
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

bool FileWatcherBaseWindows::platformDisplayDirectory(std::string directory)
{

    //get the wide-character file-path
    WCHAR wDirectoryName[_MAX_PATH];
    char fileName[_MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, directory.c_str(), _MAX_PATH, wDirectoryName, _MAX_PATH);

    WIN32_FIND_DATA fdFileData;
    HANDLE hFile = FindFirstFile(wDirectoryName, &fdFileData);

    //check if handle is ok
    if(hFile == INVALID_HANDLE_VALUE)
    {

        log("Warning: Directory could not be displayed:" + directory, _MAX_PATH);
        platformPrintColorS(directory + " could not be displayed properly", {Colors::RED, Colors::INTENSE, Colors::B_RED});
        std::cout << std::endl;
        return false;
    }

    else
    {
        do
        {

            //convert current filename into narrow character and change color based on FileType
            WideCharToMultiByte(CP_UTF8, 0, fdFileData.cFileName, _MAX_PATH, fileName, _MAX_PATH, NULL, NULL);

            switch(platformFileType(fileName))
            {
            case FileType::ftHEADER:
                platformPrintColorC(fileName, {Colors::BLUE, Colors::GREEN});
                break;
            case FileType::ftFILE:
                platformPrintColorC(fileName, {Colors::RED, Colors::BLUE});
                break;
            case FileType::ftFOLDER:
                platformPrintColorC(fileName, {Colors::BLUE});
                break;
            case FileType::ftIMPLEMENTATION:
                platformPrintColorC(fileName, {Colors::GREEN});
                break;
            default:
                platformPrintColorC(fileName, {Colors::RED, Colors::GREEN});
                break;
            }

            std::cout << std::endl;

        }while(FindNextFile(hFile, &fdFileData));

        FindClose(hFile);

        return true;
    }
}

void FileWatcherBaseWindows::platformBeginDirectoryWatch()
{
    /*
    //Create security attributes object to suspend and resume thread
    SECURITY_ATTRIBUTES saAttribute;
    PSECURITY_DESCRIPTOR pSDescriptor = NULL;

    pSDescriptor = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR,
                                                  SECURITY_DESCRIPTOR_MIN_LENGTH);

    if(pSDescriptor == NULL)
    {
        log("Unable to initialize thread. Security Descriptor returned NULL", 50);
        ExitProcess(EXIT_FAILURE);
    }

    if(!InitializeSecurityDescriptor(pSDescriptor, SECURITY_DESCRIPTOR_REVISION))
    {
        log("Unable to initialize thread. Security Descriptor not intialized", 50);
        ExitProcess(EXIT_FAILURE);
    }

    if(!IsValidSecurityDescriptor(pSDescriptor))
    {
        log("Unable to intialize thread. Security Descriptor not initialized properly", 60);
        ExitProcess(EXIT_FAILURE);
    }
    */

    //spawn worker thread
    m_hWatcherThread = CreateThread(
                                  NULL,
                                  0,
                                  watchDirectoryEntryPoint,
                                  this,
                                  0,
                                  &m_dwThreadId);

    //check if worker thread spawn successfully
    if(m_hWatcherThread == NULL)
    {

        //create log file and log the error, close program
        log("Worker thread could not be spawned. Exiting with error code 1", 100);
        ExitProcess(EXIT_FAILURE);
    }
}

FileType FileWatcherBaseWindows::platformFileType(std::string filename)
{

    if(FileExtensions.find(wstring_to_utf8(PathFindExtension(
                     utf8_to_wstring(filename).c_str()
                                             ))) != FileExtensions.end())
    {
        return FileExtensions.at(wstring_to_utf8(PathFindExtension(utf8_to_wstring(filename).c_str())));
    }

    else return FileType::ftFILE;
}

void FileWatcherBaseWindows::watchDirectory()
{

    platformPrintColorS("Currently watching '" + M_config.currentlyWatchedDirectory + "'...", {Colors::BLUE, Colors::GREEN});

    static unsigned attempts = 0;

    if(M_hDirectoryStatus == INVALID_HANDLE_VALUE)
    {
        log("Unable to create watchpoint in directory '" + M_config.currentlyWatchedDirectory + "'", 200);
        ExitProcess(EXIT_FAILURE);
    }

    do
    {
        //query a change in the current directory
        M_hDirectoryStatus = FindFirstChangeNotification(
                                    utf8_to_wstring(M_config.currentlyWatchedDirectory).c_str(),
                                    true,
                                    FILE_NOTIFY_CHANGE_FILE_NAME
                                    );

        //if a change has been found, set the active boolean and dump the information to a FindData struct
        DWORD dwStatus = WaitForSingleObject(M_hDirectoryStatus, INFINITE);

        PFILE_NOTIFY_INFORMATION pInfo;

        switch(dwStatus)
        {

        case WAIT_OBJECT_0:
            //A file has been modified, we take appropriate measures
            if(ReadDirectoryChangesW(
                                     M_hDirectoryStatus,
                                     pInfo,
                                     sizeof(FILE_NOTIFY_INFORMATION),
                                     true,
                                     FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME,
                                     0,
                                     NULL,
                                     NULL
                                     ))
            {
                switch(pInfo->Action)
                {
                case FILE_ACTION_ADDED:
                    handleFileState(FileHandleState::ADDED, wstring_to_utf8(pInfo->FileName, pInfo->FileNameLength));
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            log("Notification handle was invalid", 20);
            ExitProcess(EXIT_FAILURE);
            break;
        }

        if(!FindNextChangeNotification(M_hDirectoryStatus))
        {
            log("Warning: Unable to request additional change notification; Attempt #" + attempts, 30);
            ++attempts;

            if(attempts >=3)
            {
                log("Unable to request change in directory '" + M_config.currentlyWatchedDirectory + "'", _MAX_PATH);
                ExitProcess(EXIT_FAILURE);
            }
        }

    }while(!M_threadDead);

}

void FileWatcherBaseWindows::generateShorthandList()
{

}

void FileWatcherBaseWindows::openShorthandList()
{

}

void FileWatcherBaseWindows::parseShorthandList()
{

}

void FileWatcherBaseWindows::generateHeaderFile()
{

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
         stCurrentDate.wHour % 12,
         stCurrentDate.wMinute,
         (stCurrentDate.wHour > 12 ? "PM" : "AM"),
         text);

    out_file << cBuffer << "\n" << GetLastError() << std::endl;
    out_file.close();

    return true;
}

void FileWatcherBaseWindows::platformThreadedWait(unsigned milliseconds)
{
    HANDLE hCurrentThread = GetCurrentThread();
    WaitForSingleObject(hCurrentThread, milliseconds);
}

void FileWatcherBaseWindows::pollWatcherThread()
{
    std::cout << "polling!";
}
