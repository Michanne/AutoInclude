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

    char fileName[_MAX_PATH];

    //get the wide-character file-path using utf8_to_wstring
    WIN32_FIND_DATA fdFileData;
    HANDLE hFile = FindFirstFile(utf8_to_wstring(directory).c_str(), &fdFileData);

    //check if handle is ok
    if(hFile == INVALID_HANDLE_VALUE)
    {

        log(AUTO_INCLUDE_WARN_DIR_FAIL_DISPLAY(directory), _MAX_PATH);
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
    }
    return true;
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
        log(AUTO_INCLUDE_ERROR_THREAD_FAIL_SPAWN, 100);
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


/**
Thank jesus christ for Jim Beveridge who was kind enough to write an enormously detailed 2-part guide on ReadDirectoryChangesW
http://qualapps.blogspot.com/2010/05/understanding-readdirectorychangesw.html
**/
void FileWatcherBaseWindows::watchDirectory()
{

    platformPrintColorS("Currently watching '" + M_config.currentlyWatchedDirectory + "'...", {Colors::BLUE, Colors::GREEN});

    static unsigned attempts = 0;

    if(M_hDirectoryStatus == INVALID_HANDLE_VALUE)
    {
        log(AUTO_INCLUDE_ERROR_WATCHPOINT_FAIL_SPAWN(M_config.currentlyWatchedDirectory), _MAX_PATH + 30);
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

        FILE_NOTIFY_INFORMATION fniInfo;

        switch(dwStatus)
        {

        case WAIT_OBJECT_0:
            //A file has been modified, we take appropriate measures
            if(ReadDirectoryChangesW(
                                     M_hDirectoryStatus,
                                     (LPVOID)&fniInfo,
                                     sizeof(fniInfo),
                                     true,
                                     FILE_NOTIFY_CHANGE_FILE_NAME |
                                     FILE_NOTIFY_CHANGE_DIR_NAME  |
                                     FILE_NOTIFY_CHANGE_CREATION,
                                     0,
                                     NULL,
                                     NULL
                                     ))
            {
                std::cout << "check";
                std::cout << ": " << wstring_to_utf8(fniInfo.FileName, fniInfo.FileNameLength) << std::endl;
                switch(fniInfo.Action)
                {
                case FILE_ACTION_ADDED:
                    handleFileState(FileHandleState::ADDED, wstring_to_utf8(fniInfo.FileName, fniInfo.FileNameLength));
                    break;
                case FILE_ACTION_REMOVED:
                    handleFileState(FileHandleState::REMOVED, wstring_to_utf8(fniInfo.FileName, fniInfo.FileNameLength));
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            log(AUTO_INCLUDE_ERROR_NOTIFY_INVALID, 20);
            ExitProcess(EXIT_FAILURE);
            break;
        }

        if(!FindNextChangeNotification(M_hDirectoryStatus))
        {
            log(AUTO_INCLUDE_WARN_NOTIFY_UPDATE_FAIL(attempts), 30);
            ++attempts;

            if(attempts >=3)
            {
                log(AUTO_INCLUDE_ERROR_NOTIFY_FAIL(M_config.currentlyWatchedDirectory), _MAX_PATH + 30);
                ExitProcess(EXIT_FAILURE);
            }
        }

    }while(!M_threadDead);

}


void FileWatcherBaseWindows::platformGenerateHeaderFile()
{

}

void FileWatcherBaseWindows::platformOpenShorthandList()
{

}

void FileWatcherBaseWindows::generateShorthandList()
{

}

void FileWatcherBaseWindows::parseShorthandList()
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

void FileWatcherBaseWindows::platformReadyAIDirectory()
{

    DWORD dwAttrib = GetFileAttributes(utf8_to_wstring(M_config.currentlyWatchedDirectory + "/" + AUTO_INCLUDE_BASE_PATH).c_str());

    //check if directory exists, if it doesn't, create it
    if(dwAttrib == INVALID_FILE_ATTRIBUTES ||
       ~(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
    {
        BOOL success = CreateDirectory(utf8_to_wstring(M_config.currentlyWatchedDirectory + "/" + AUTO_INCLUDE_BASE_PATH).c_str(), NULL);
        switch(success)
        {
        case ERROR_ALREADY_EXISTS:
            log(AUTO_INCLUDE_WARN_AI_DIR_EXIST(M_config.currentlyWatchedDirectory + "/" + AUTO_INCLUDE_BASE_PATH), _MAX_PATH + 20);
            break;
        case ERROR_PATH_NOT_FOUND:
            break;
        default:
            return;
        }
    }

    else return;
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
