#pragma once

#include "event.h"
#include "event2/listener.h"
#include "event2/thread.h"
#include "gbCommon.h"
#include <thread>

class gbSvrCore
{
public:
	static bool Initialize();
	static void Run(const unsigned int port);
	static void Shutdown();
private:
	static event_base* _base;
	static evconnlistener* _listener;

	//note!!!, reversing each number  bytes order in bufffer individually not reversing the whole buffer one time, when necessary 
	static bool _is_little_endian;
	static std::thread* _logicLoopThread;
private:
	static void _accept_conn_cb(evconnlistener* listener, evutil_socket_t fd, sockaddr* address, int socklen, void* ctx);
	static void _accept_error_cb(evconnlistener* listerner, void*ctx);
	static void _read_cb(bufferevent* bev, void* ctx);
	static void _event_cb(bufferevent* bev, short events, void* ctx);
	static void _log_callback(int severity, const char* msg);
	static void _fatal_error_callback(int err);

	static void _svr_core_thread(void* p);
};
