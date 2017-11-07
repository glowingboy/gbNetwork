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

#include "gbIOTunnel.h"
#include "gbIOEventHandler.h"

class gbIOEvent
{
    SingletonDeclare(gbIOEvent);
    ~gbIOEvent();
public:
    //@param szLocalIP, if szLocalIP == nullptr, then it should only listen loopback interface
    bool Start(const char* szLocalIP, unsigned short port);


    //should be called in the same thread as IOEvent
    void Connect(evutil_socket_t socket, const char* IP, unsigned short port);

    gbIOTunnelMgr& GetIOTunnelMgr(){ return _ioTunnelMgr; }
    gbIOEventHandler& GetIOEventHandler(){ return _ioEventHandler;}
    void Shutdown();
private:
    gbIOTunnelMgr _ioTunnelMgr;
    gbIOEventHandler _ioEventHandler;
    
    evutil_socket_t _svrSocketFd;
    //only watch loopback interface connection
    evutil_socket_t _watchdogSvrSocketFd;//todo

    std::list<event*> _lstEvs;
    event_base* _base;
    evconnlistener* _listener;
    evconnlistener* _watchdogListener;
    std::thread* _dispatchThread;

private:


    
    static void _log_callback(int severity, const char* msg);
    static void _fatal_error_callback(int err);
    static void _ev_cb(evutil_socket_t fd, short what, void* arg);
    static void _listener_cb(evconnlistener* listener, evutil_socket_t sock, sockaddr* addr, int socklen, void* ptr);

    static void _watchdog_ev_cb(evutil_socket_t fd, short what, void* arg);
    static void _watchdog_listener_cb(evconnlistener* listener, evutil_socket_t sock, sockaddr* addr, int socklen, void* ptr);

    static void _listener_error_cb(evconnlistener* listener, void* ptr);

};
