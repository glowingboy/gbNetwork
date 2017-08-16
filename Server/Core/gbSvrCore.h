#pragma once

#ifdef __GNUC__
#include <arpa/inet.h>
#endif

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
	static bool _is_little_endian;
private:
	static void _bytes_reverse(char* d, const size_t len);
	static void _accept_conn_cb(evconnlistener* listener, evutil_socket_t fd, sockaddr* address, int socklen, void* ctx);
	static void _accept_error_cb(evconnlistener* listerner, void*ctx);
	static void _read_cb(bufferevent* bev, void* ctx);
	static void _event_cb(bufferevent* bev, short events, void* ctx);
};
