#pragma once

#include "event.h"
#include "event2/listener.h"
#include "event2/thread.h"
#include "gbCommon.h"
#include "Log/gbLog.h"
#include "String/gbString.h"
//#include "gbUDPDataHandler.h"
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_map>

class gbTCPSocketData
{
public:
    inline gbTCPSocketData(const evutil_socket_t socket):
	_socket(socket)
	{}
    inline std::mutex& GetMtx(){ return _mtx; }
    inline std::vector<unsigned char>& GetRemainderData(){ return _remainderData; }
    inline evutil_socket_t GetSocket() { return _socket; }
private:
    std::mutex _mtx;
    std::vector<unsigned char> _remainderData;
    const evutil_socket_t _socket;
};

class gbSvrNet
{
    SingletonDeclare(gbSvrNet);
public:
    bool Start(const char* szLocalIP, unsigned short port);

private:
    bool _is_little_endian;
    //  static unsigned char _recvBuffer[gb_UDP_MAX_PACKET_SIZE];
    evutil_socket_t _sockfd;
    evutil_socket_t _watchdogSockfd;
    // event* _ev;
    // event* _watchdogEv;
    std::vector<event*> _evs;
    event_base* _base;
    evconnlistener* _listener;
    evconnlistener* _watchdog_listener;
    std::thread* _dispathThread;
    typedef std::unordered_map<evutil_socket_t, gbTCPSocketData*>::iterator TCPSocketDataItr;
    std::unordered_map<evutil_socket_t, gbTCPSocketData*> _mpTCPSocketDatas;
private:
    //controlled by watchdog
    void Shutdown();
    static void _log_callback(int severity, const char* msg);
    static void _fatal_error_callback(int err);
    static void _ev_cb(evutil_socket_t fd, short what, void* arg);
    static void _watchdog_ev_cb(evutil_socket_t fd, short what, void* arg);
    static void _listener_cb(evconnlistener* listener, evutil_socket_t sock, sockaddr* addr, int socklen, void* ptr);
    static void _listener_error_cb(evconnlistener* listener, void* ptr);

    static void _watchdog_listener_cb(evconnlistener* listener, evutil_socket_t sock, sockaddr* addr, int socklen, void* ptr);
    static void _watchdog_listener_error_cb(evconnlistener* listener, void* ptr);

};
