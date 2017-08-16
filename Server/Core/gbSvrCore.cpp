#include "gbSvrCore.h"
#include "Log/gbLog.h"
#include "String/gbString.h"

event_base* gbSvrCore::_base;
evconnlistener* gbSvrCore::_listener;
bool gbSvrCore::_is_little_endian;
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

	//check endian
	int t = 1;
	_is_little_endian = ((char*)&t)[0];

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

void gbSvrCore::_bytes_reverse(char* d, const size_t len)
{
	int m = len / 2;
	for (int i = 0; i < m; i++)
	{
		char& l = d[i];
		char& r = d[len - i - 1];
		char tmp = l;
		l = r;
		r = tmp;
	}
}

void gbSvrCore::_accept_conn_cb(evconnlistener* listener, evutil_socket_t fd, sockaddr* address, int socklen, void* ctx)
{
	sockaddr_in* addr = (sockaddr_in*)address;
	gbLog::Instance().Log(gbString("on connected@ ") + inet_ntoa(addr->sin_addr));

	event_base* base = evconnlistener_get_base(listener);
	bufferevent* bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(bev, _read_cb, NULL, _event_cb, NULL);
	bufferevent_enable(bev, EV_READ | EV_WRITE);
}

void gbSvrCore::_accept_error_cb(evconnlistener* listerner, void*ctx)
{
	event_base* base = evconnlistener_get_base(listerner);
	int err = EVUTIL_SOCKET_ERROR();
	gbLog::Instance().Error(gbString("error on listener: ") + evutil_socket_error_to_string(err));
	event_base_loopexit(base, NULL);
}

void gbSvrCore::_read_cb(bufferevent* bev, void* ctx)
{
	//evbuffer* input = bufferevent_get_input(bev);

	//evbuffer* output = bufferevent_get_output(bev);

	//evbuffer_add_buffer(output, input);

	char tmp[1024] = { '\0' };
	size_t len = bufferevent_read(bev, tmp, 1024);
	if (_is_little_endian)
		_bytes_reverse(tmp, len);
	tmp[len] = '\0';
	gbLog::Instance().Log(gbString("on reead:") + tmp);
	
	if (_is_little_endian)
		_bytes_reverse(tmp, len);
	bufferevent_write(bev, tmp, len);
}


void gbSvrCore::_event_cb(bufferevent* bev, short events, void* ctx)
{
	if (events & BEV_EVENT_ERROR)
		gbLog::Instance().Error("BEV_EVENT_ERROR");
	if (events & BEV_EVENT_EOF)
		gbLog::Instance().Error("BEV_EVENT_EOF");
}
