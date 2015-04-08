
template <typename FUNCTION>
WatchEventHandle FileWatcher::delegateNewWatchEvent(WatchInstanceHandle handle, WatchFileOption option, FUNCTION func)
{

    WatchInstance* thisInstance = directoryNames.at(handle);
    WatchEventHandle a = thisInstance->allocEvent.getHandle();
    WatchEventInformation* b = new WatchEventInformation();

    b->currentOptions |= option;
    b->function = std::bind(func, option);

    thisInstance->watchDelegates.push_back(b);
    return a;
}
