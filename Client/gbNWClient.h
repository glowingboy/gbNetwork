#pragma once
#include "gbCommon.h"
#include "../gbType.h"
#include <thread>

#include "event.h"
#include "event2/listener.h"

class gbNWClient
{
public:
    bool Initialize(const unsigned short watchdogPort);
    void Connect(const char* szIP, const unsigned short port);
    void Close();
private:
    gb_socket_t _socket;
    event_base* _evBase;
    event* _ev;
    gb_socket_t _watchdogSvrSocket;
    gb_socket_t _watchdogClntSocket;
    evconnlistener* _watchdogListener;
    event* _watchdogSvr_ev;
    event* _watchdogClnt_ev;
    std::thread* _dispatchThread;
private:
    static void _log_callback(int severity, const char* msg);
    static void _fatal_error_callback(int err);
    static void _ev_cb(evutil_socket_t fd, short what, void* arg);

    static void _watchdogListener_cb(evconnlistener* listener, evutil_socket_t sock, sockaddr* addr, int socklen, void* ptr);

    static void _watchdogListener_error_cb(evconnlistener* listener, void* ptr);
};
