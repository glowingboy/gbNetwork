#include "gbSvrCore.h"

#ifdef __GNUC__
#include <arpa/inet.h>
#endif


#include "gbUtils/include/gbLog.h"
#include "gbUtils/include/gbString.h"
#include "ThreadPool/gbThreadPool.h"

#include "gbSvrLogicLoop.h"
#include "gbUtils/include/gbLuaCPP.h"
#include "gbUtils/include/gbFileSystem.h"
#include "../gbLuaState.h"
event_base* gbSvrCore::_base;
evconnlistener* gbSvrCore::_listener;
bool gbSvrCore::_is_little_endian;
std::thread* gbSvrCore::_logicLoopThread;
lua_State* gbSvrCore::_L;
unsigned char gbSvrCore::_recvBuffer[gb_UDP_MAX_PACKET_SIZE] = {0};
evutil_socket_t gbSvrCore::_sockfd;
event* gbSvrCore::_ev;
gbString gbSvrCore::_workPath;
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

#ifdef _MSC_VER
	if (evthread_use_windows_threads() == -1)
	{
		gbLog::Instance().Error("evthread_use_windows_threads");
		return false;
	}
#elif __GNUC__
	if (evthread_use_pthreads() == -1)
	{
		gbLog::Instance().Error("evthread_use_windows_threads");
		return false;
	}
#endif


	if (!gbThreadPool::Instance().Initialize(-1))
	{
		gbLog::Instance().Error("thread pool initialize error");
		return false;
	}

	event_set_log_callback(_log_callback);

#ifdef _DEBUG
	event_enable_debug_mode();
#endif

	event_set_fatal_callback(_fatal_error_callback);


	_base = event_base_new();
	if (_base == nullptr)
		return false;
	//check endian
	int t = 1;
	_is_little_endian = ((char*)&t)[0];

	//logic loop
	_logicLoopThread = new std::thread(gbSvrLogicLoop::Loop);

	//load lua script
	_L = gbLuaCPP_init();
	if (_L == nullptr)
		return false;

	gbLuaStateMgr::Instance().SetAccInfoState(_L);
	
	gbFileSystem::Instance().GetWorkPath(_workPath);

	gbLuaCPP_appendPackagePath(_L, _workPath + "../../Server/Script/");
	gbLuaCPP_appendPackagePath(_L, _workPath + "../../Server/Data/");
	
	if (!gbLuaCPP_dofile(_L, _workPath + "../../Server/Script/gbSvrCore.lua"))
	    return false;

	return true;
}

void gbSvrCore::Shutdown()
{
//    ::close(_sockfd);
    event_free(_ev);
    gbSAFE_DELETE(_logicLoopThread);
}

void gbSvrCore::Run(const char* port)
{
#ifdef __GNUC__
    addrinfo hints, *svrinfo;
    ::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    int ret = ::getaddrinfo(NULL, port, &hints, &svrinfo);
    if(ret != 0)
    {
	gbLog::Instance().Error((gbString)"getaddrinfo err@" + strerror(errno));
	return;
    }
    _sockfd = socket(svrinfo->ai_family, svrinfo->ai_socktype, svrinfo->ai_protocol);
    if(_sockfd == -1)
    {
	gbLog::Instance().Error(gbString("sockfd create err@") + strerror(errno));
	return;
    }

    if(bind(_sockfd, svrinfo->ai_addr, svrinfo->ai_addrlen) == -1)
    {
	gbLog::Instance().Error(gbString("bind err@") + strerror(errno));
	return;
    }

    _ev = event_new(_base, _sockfd, EV_READ | EV_WRITE | EV_PERSIST | EV_ET, _ev_cb, NULL);
    event_add(_ev, NULL);
    
#elif _MSC_VER
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
#endif

	event_base_dispatch(_base);
	event_base_free(_base);
	
}

void gbSvrCore::_ev_cb(evutil_socket_t fd, short what, void* arg)
{
    if(what & EV_READ)
    {
	sockaddr* srcSockAddr = new sockaddr;
	static socklen_t addrLen = sizeof(sockaddr);
	size_t len = recvfrom(fd, _recvBuffer, gb_UDP_MAX_PACKET_SIZE, 0, srcSockAddr, &addrLen);
	if(len != -1)
	{
	    unsigned char* rBuf = new unsigned char[len];
	    //release after decoded
	    memcpy(rBuf, _recvBuffer, len);

	    gbUDPData *ud = new gbUDPData(rBuf, len, srcSockAddr, addrLen);
	    
//	    gbUDPDataMgr::Instance().Push(rBuf, len);
	    gbUDPDataMgr::Instance().Push(ud);
	}
	else
	    gbLog::Instance().Error((gbString)"recvfrom err@" + strerror(errno));
    }
    else if (what & EV_WRITE)
    {
	gbLog::Instance().Log("EV_WRITE");
    }
}
void gbSvrCore::_svr_core_thread(void* p)
{
	event_base_dispatch((event_base*)p);
	event_base_free((event_base*)p);
}
// void gbSvrCore::_accept_conn_cb(evconnlistener* listener, evutil_socket_t fd, sockaddr* address, int socklen, void* ctx)
// {
// 	sockaddr_in* addr = (sockaddr_in*)address;
// 	char strAddr[64] = { '\0' };
// 	inet_ntop(AF_INET, &(addr->sin_addr), (PSTR)strAddr, 64);
// 	gbLog::Instance().Log(gbString("on connected@ ") + strAddr);

// 	//event_base* base = evconnlistener_get_base(listener);

// 	event_base* base = event_base_new();
// 	if (base != nullptr)
// 	{
// 		bufferevent* bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
// 		bufferevent_setcb(bev, _read_cb, NULL, _event_cb, NULL);
// 		bufferevent_enable(bev, EV_READ | EV_WRITE);
// 		gbThreadPool::Instance().PushTask(gbTask(_svr_core_thread, base));
// 	}
// 	else
// 		gbLog::Instance().Error("event_base_new error");

// }

// void gbSvrCore::_accept_error_cb(evconnlistener* listerner, void*ctx)
// {
// 	event_base* base = evconnlistener_get_base(listerner);
// 	int err = EVUTIL_SOCKET_ERROR();
// 	gbLog::Instance().Error(gbString("error on listener: ") + evutil_socket_error_to_string(err));
// 	event_base_loopexit(base, NULL);
// }

// void gbSvrCore::_read_cb(bufferevent* bev, void* ctx)
// {
// 	//evbuffer* input = bufferevent_get_input(bev);

// 	//evbuffer* output = bufferevent_get_output(bev);

// 	//evbuffer_add_buffer(output, input);

// 	const size_t length = evbuffer_get_length(bev->input);
// 	//release after decoded
// 	unsigned char* buffer = new unsigned char[length];
// 	bufferevent_read(bev, buffer, length);
// 	gbRawDataMgr::Instance().Push(buffer, length);
// 	return;

// 	char tmp[1024] = { '\0' };
// 	size_t len = bufferevent_read(bev, tmp, 1024);

// 	len = len == 1024 ? 1023 : len;
// 	tmp[len] = '\0';
// 	gbLog::Instance().Log(gbString("on reead:") + tmp);

// 	bufferevent_write(bev, tmp, len);
// }


// void gbSvrCore::_event_cb(bufferevent* bev, short events, void* ctx)
// {
// 	if (events & BEV_EVENT_ERROR)
// 		gbLog::Instance().Error("BEV_EVENT_ERROR");
// 	if (events & BEV_EVENT_EOF)
// 		gbLog::Instance().Error("BEV_EVENT_EOF");
// }

void gbSvrCore::_log_callback(int severity, const char* msg)
{
	gbLog::Instance().Log(gbString("severity:") + severity + ", msg:");
}

void gbSvrCore::_fatal_error_callback(int err)
{
	gbLog::Instance().Error(gbString("fatal error@") + err);
}
