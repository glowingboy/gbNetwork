#pragma once
#include "gbCommon.h"
#include "../gbType.h"
#include <thread>
#include "event.h"

class gbNWClient
{
public:
    bool Initialize();
    void Connect(const char* szIP, const unsigned short port);
    void Close();
private:
    gb_socket_t _socket;
    event_base* _evBase;
    event* _ev;
    std::thread* _dispatchThread;
private:
    static void _log_callback(int severity, const char* msg);
    static void _fatal_error_callback(int err);
    static void _ev_cb(evutil_socket_t fd, short what, void* arg);

};
