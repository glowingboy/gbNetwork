#pragma once
#include "gbCommon.h"
#include "../gbType.h"
#include <thread>

#include "event.h"
#include "event2/listener.h"
#include <queue>
#include <mutex>
#include <condition_variable>

//base: recvs data only when I want to
//advanced:always recvs data when data is comming in a loop thread, and has a watchdogSvr for controlling this client

class gbBaseClient
{
public:
    static bool Initialize();
public:
    virtual bool Setup();
    void Connect(const char* szIP, const unsigned short port);
private:
    gb_socket_t _socket;
    event_base* _evBase;
    event* _ev;
    std::thread* _dispatchThread;
    std::queue<gb_send_pkg*> _qSendPkg;
    std::mutex _sendCVMtx;
    std::condition_variable _sendCV;
    bool _writable;
private:
    static void _log_callback(int severity, const char* msg);
    static void _fatal_error_callback(int err);
    static void _ev_cb(evutil_socket_t fd, short what, void* arg);
    static void _send_thread(gbBaseClient* clnt);
};

class gbAdvancedClient: public gbBaseClient
{
public:
    virtual bool Initialize();
    bool InitializeWatchdog(const unsigned short watchdogPort);
private:
    
};

class gbNWClient
{
public:
    bool Initialize(const unsigned short watchdogPort);

    void Close();
    void Send(gb_send_pkg* sPkg);
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

    std::queue<gb_send_pkg*> _qSendPkg;
    std::mutex _sendCVMtx;
    std::condition_variable _sendCV;
    bool _writable;
private:
    static void _log_callback(int severity, const char* msg);
    static void _fatal_error_callback(int err);
    static void _ev_cb(evutil_socket_t fd, short what, void* arg);

    static void _watchdogListener_cb(evconnlistener* listener, evutil_socket_t sock, sockaddr* addr, int socklen, void* ptr);

    static void _watchdogListener_error_cb(evconnlistener* listener, void* ptr);

    static void _send_thread(gbNWClient* clnt);
};

typedef gbAdvancedClient gbClient;
