#pragma once

#include "event.h"
#include "event2/listener.h"
#include "event2/thread.h"
#include "gbUtils/include/gbCommon.h"
#include <thread>
#include "gbUtils/include/gbLuaCPP.h"
#include "../../gbUDPData.h"
#include "gbUtils/include/gbString.h"
class gbSvrCore
{
public:
    static bool Initialize();
    static void Run(const char* port);
    static void Shutdown();
private:
    static event_base* _base;
    static evconnlistener* _listener;

    //note!!!, reversing each number  bytes order in bufffer individually not reversing the whole buffer one time, when necessary 
    static bool _is_little_endian;
    static std::thread* _logicLoopThread;

    static lua_State* _L;
    static unsigned char _recvBuffer[gb_UDP_MAX_PACKET_SIZE];
    static evutil_socket_t _sockfd;
    static event* _ev;
    static gbString _workPath;
private:
//    static void _accept_conn_cb(evconnlistener* listener, evutil_socket_t fd, sockaddr* address, int socklen, void* ctx);
//    static void _accept_error_cb(evconnlistener* listerner, void*ctx);
    // static void _read_cb(bufferevent* bev, void* ctx);
    // static void _event_cb(bufferevent* bev, short events, void* ctx);
    static void _log_callback(int severity, const char* msg);
    static void _fatal_error_callback(int err);

    static void _svr_core_thread(void* p);

    static void _ev_cb(evutil_socket_t fd, short what, void* arg);
};
