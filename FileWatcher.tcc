
template <typename FUNCTION>
WatchEventHandle FileWatcher::delegateNewWatchEvent(WatchInstanceHandle handle, WatchFileOption option, FUNCTION func)
{

    WatchInstance* thisInstance = directoryNames.at(handle);
    WatchEventInformation* b = new WatchEventInformation();

    b->currentOptions |= option;
    b->function = std::bind(func, option);

    return thisInstance->addEvent(b);;
}
