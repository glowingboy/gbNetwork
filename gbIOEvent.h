#pragma once

#include "event.h"
#include "event2/listener.h"
#include "event2/thread.h"
#include "gbCommon.h"
#include "Log/gbLog.h"
#include "String/gbString.h"
#include <thread>
#include <mutex>
#include <list>

class gbIOEvent
{
    SingletonDeclare(gbIOEvent);

public:
    bool Start(const char* szLocalIP, unsigned short port);

    evutil_socket_t _svrSocketFd;
    evutil_socket_t _watchdogSvrSocketFd;//todo

    std::list<event*> _lstEvs;
    event_base* _base;
    evconnlistener* _listener;
    std::thread* _dispatchThread;

private:

    void Shutdown();
    
    static void _log_callback(int severity, const char* msg);
    static void _fatal_error_callback(int err);
    static void _ev_cb(evutil_socket_t fd, short what, void* arg);
    static void _listener_cb(evconnlistener* listener, evutil_socket_t sock, sockaddr* addr, int socklen, void* ptr);
    static void _listener_error_cb(evconnlistener* listener, void* ptr);

};
