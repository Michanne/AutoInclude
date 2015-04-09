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

void FileWatcherBaseWindows::platformCloseThread()
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
        log(FILE_WATCHER_WARN_DIR_FAIL_DISPLAY(directory), _MAX_PATH);
        platformPrintColorS(directory + " could not be displayed properly", FILE_WATCHER_COLOR_DIR_FAIL_DISPLAY);
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
                platformPrintColorC(fileName, FILE_WATCHER_COLOR_FILE_HEADER);
                break;
            case FileType::ftFILE:
                platformPrintColorC(fileName, FILE_WATCHER_COLOR_FILE_FILE);
                break;
            case FileType::ftFOLDER:
                platformPrintColorC(fileName, FILE_WATCHER_COLOR_FILE_FOLDER);
                break;
            case FileType::ftIMPLEMENTATION:
                platformPrintColorC(fileName, FILE_WATCHER_COLOR_FILE_IMPLEMENTATION);
                break;
            }

            std::cout << std::endl;
        }while(FindNextFile(hFile, &fdFileData));
        FindClose(hFile);
    }
    return true;
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

I'll fix this function once I understand all these nuances. For now, this is incomplete
**/
void FileWatcherBaseWindows::watchDirectory()
{
    platformPrintColorS("Currently watching '" + M_config.currentlyWatchedDirectory + "'...", FILE_WATCHER_COLOR_DIR_WATCH);

    m_hDirectoryStatus = CreateFile(
        m_config.currentlyWatchedDirectory.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ
         | FILE_SHARE_WRITE
         | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED
        );

    if(m_hDirectoryStatus == INVALID_HANDLE_VALUE)
    {
        log(FILE_WATCHER_ERROR_WATCHPOINT_FAIL_SPAWN(m_config.currentlyWatchedDirectory), 40);
        ExitProcess(EXIT_FAILURE);
    }

    do
    {


    }while(!M_threadDead);

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

void FileWatcherBaseWindows::platformCreateDirectory(std::string dirName)
{
    DWORD dwAttrib = GetFileAttributes(utf8_to_wstring(M_config.currentlyWatchedDirectory + "/" + dirName).c_str());

    //check if directory exists, if it doesn't, create it
    if(dwAttrib == INVALID_FILE_ATTRIBUTES ||
       ~(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
    {
        BOOL success = CreateDirectory(utf8_to_wstring(M_config.currentlyWatchedDirectory + "/" + dirName).c_str(), NULL);
        switch(success)
        {
        case ERROR_ALREADY_EXISTS:
            log(FILE_WATCHER_WARN_DIR_EXIST(M_config.currentlyWatchedDirectory + "/" + dirName), _MAX_PATH + 20);
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
