
template <typename FUNCTION>
void WatchInstance::platformCreateThread(FUNCTION f)
{

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
        log(FILE_WATCHER_ERROR_THREAD_FAIL_SPAWN, 100);
        ExitProcess(EXIT_FAILURE);
    }
}
