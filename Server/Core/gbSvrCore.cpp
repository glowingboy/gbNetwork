#include "gbSvrCore.h"
#include "Log/gbLog.h"
#include "String/gbString.h"

event_base* gbSvrCore::_base;
evconnlistener* gbSvrCore::_listener;

bool gbSvrCore::Initialize()
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	int err;
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		gbLog::Instance().Error(gbString("WSAStartup failed with error: ") + err);
		return false;
	}
#endif

	_base = event_base_new();
	return _base == nullptr ? false : true;
}

void gbSvrCore::Run(const unsigned int port)
{
	sockaddr_in sin;

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(0);
	sin.sin_port = htons(port);

	_listener = evconnlistener_new_bind(_base, _accept_conn_cb, NULL, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1, (sockaddr*)&sin, sizeof(sin));
	if (!_listener)
	{
		gbLog::Instance().Error("failed to new listener!");
		return;
	}
	evconnlistener_set_error_cb(_listener, _accept_error_cb);
	event_base_dispatch(_base);
}

void gbSvrCore::_accept_conn_cb(evconnlistener* listener, evutil_socket_t fd, sockaddr* address, int socklen, void* ctx)
{
	sockaddr_in* addr = (sockaddr_in*)address;
	std::cout << "on connected to" << inet_ntoa(addr->sin_addr) << std::endl;
	//event_base* base = evconnlistener_get_base(listener);
	//bufferevent* bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	//bufferevent_setcb(bev, echo_read_cb, NULL, echo_event_cb, NULL);
	//bufferevent_enable(bev, EV_READ | EV_WRITE);
}

void gbSvrCore::_accept_error_cb(evconnlistener* listerner, void*ctx)
{

}