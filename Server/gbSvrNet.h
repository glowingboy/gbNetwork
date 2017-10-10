#pragma once

#include "event.h"
#include "event2/listener.h"
#include "event2/thread.h"
#include "gbCommon.h"
#include "Log/gbLog.h"
#include "String/gbString.h"
#include "gbUDPDataHandler.h"
#include <thread>
class gbSvrNet
{
    SingletonDeclare(gbSvrNet);
public:
    bool Start(const char* szLocalIP, unsigned short port);

private:
    bool _is_little_endian;
    static unsigned char _recvBuffer[gb_UDP_MAX_PACKET_SIZE];
    evutil_socket_t _sockfd;
    evutil_socket_t _watchdogSockfd;
    event* _ev;
    event* _watchdogEv;
    event_base* _base;
    std::thread* _dispathThread;
private:
    //controlled by watchdog
    void Shutdown();
    static void _log_callback(int severity, const char* msg);
    static void _fatal_error_callback(int err);
    static void _ev_cb(evutil_socket_t fd, short what, void* arg);
    static void _watchdog_ev_cb(evutil_socket_t fd, short what, void* arg);
    
};
