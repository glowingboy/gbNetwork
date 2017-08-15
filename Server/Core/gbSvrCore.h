#pragma once 

#include "event.h"
#include "event2/listener.h"

#include "gbCommon.h"


class gbSvrCore
{
public:
	static bool Initialize();
	static void Run(const unsigned int port);
private:
	static event_base* _base;
	static evconnlistener* _listener;
private:
	static void _accept_conn_cb(evconnlistener* listener, evutil_socket_t fd, sockaddr* address, int socklen, void* ctx);
	static void _accept_error_cb(evconnlistener* listerner, void*ctx);
};