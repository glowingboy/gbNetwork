#include "gbSvrNet.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

unsigned char gbSvrNet::_recvBuffer[gb_UDP_MAX_PACKET_SIZE] = {'\0'};
//event_base* gbSvrNet::_base;

bool gbSvrNet::Start(const char* szLocalIP, const unsigned short port)
{
#ifdef _DEBUG
    event_enable_debug_mode();
#endif
    
// #ifdef _MSC_VER
//     WORD wVersionRequested;
//     WSADATA wsaData;
//     wVersionRequested = MAKEWORD(2, 2);
//     int err;
//     err = WSAStartup(wVersionRequested, &wsaData);
//     if (err != 0) {
// 	gbLog::Instance().Error(gbString("WSAStartup failed with error: ") + err);
// 	return false;
//     }

//     if (evthread_use_windows_threads() == -1)
//     {
// 	gbLog::Instance().Error("evthread_use_windows_threads");
// 	return false;
//     }

// #endif //_MSC_VER

    // if (evthread_use_pthreads() == -1)
    // {
    // 	gbLog::Instance().Error("evthread_use_windows_threads");
    // 	return false;
    // }
    
    event_set_log_callback(_log_callback);
    event_set_fatal_callback(_fatal_error_callback);

    event_config* cfg = event_config_new();
    if(cfg == nullptr)
	return false;
    event_config_set_flag(cfg, EVENT_BASE_FLAG_NOLOCK);
    _base = event_base_new_with_config(cfg);
    event_config_free(cfg);
    
//    _base = event_base_new();
    if (_base == nullptr)
	return false;
    
    //check endian
    int t = 1;
    _is_little_endian = ((char*)&t)[0];

    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(_sockfd == -1)
    {
	gbLog::Instance().Error(gbString("sockfd create err@") + strerror(errno));
	return false;
    }

    sockaddr_in svrAddr;
    memset(&svrAddr, 0, sizeof(svrAddr));
    svrAddr.sin_family = AF_INET;
    svrAddr.sin_port = htons(port);
    
    svrAddr.sin_addr.s_addr = inet_addr(szLocalIP);
    
    int optval = 1;
    if(setsockopt(_sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)
    {
    	gbLog::Instance().Error(gbString("setsockopt SO_REUSEPORT err@") + strerror(errno));
    	return false;
    }

    if(fcntl(_sockfd, F_SETFL, O_NONBLOCK) == -1)
    {
    	gbLog::Instance().Error(gbString("fcntl err@") + strerror(errno));
    	return false;
    }

    if(bind(_sockfd, (sockaddr*)&svrAddr, sizeof(svrAddr)) == -1)
    {
    	gbLog::Instance().Error(gbString("bind err@") + strerror(errno));
    	return false;
    }

    _listener = evconnlistener_new(_base,_listener_cb, nullptr, LEV_OPT_CLOSE_ON_FREE, -1, _sockfd);
    
//    _ev = event_new(_base, _sockfd, EV_READ | EV_WRITE | EV_PERSIST | EV_ET, _ev_cb, NULL);
    
//    event_add(_ev, NULL);    

    
    //watchdog socket
    _watchdogSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(_watchdogSockfd == -1)
    {
    	gbLog::Instance().Error(gbString("watchdogSockfd create err@") + strerror(errno));
    	return false;
    }

    if(setsockopt(_watchdogSockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    {
    	gbLog::Instance().Error(gbString("setsockopt SO_REUSEADDR err@") + strerror(errno));
    	return false;
    }

    if(setsockopt(_watchdogSockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)
    {
    	gbLog::Instance().Error(gbString("setsockopt SO_REUSEPORT err@") + strerror(errno));
    	return false;
    }

    //only receive from loopback interface
    svrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if(bind(_watchdogSockfd, (sockaddr*)&svrAddr, sizeof(svrAddr)) == -1)
    {
    	gbLog::Instance().Error(gbString("bind err@") + strerror(errno));
    	return false;
    }
    // _watchdogEv = event_new(_base, _watchdogSockfd, EV_READ | EV_WRITE | EV_PERSIST | EV_ET, _watchdog_ev_cb, NULL);
    // event_add(_watchdogEv, NULL);

    _dispathThread = new std::thread([&]()
				     {
					 event_base_dispatch(_base);					 			       });
    
    return true;
}

void gbSvrNet::Shutdown()
{
    event_base_loopexit(_base, NULL);
    ::close(_sockfd);
//    event_free(_ev);

    ::close(_watchdogSockfd);
//    event_free(_watchdogEv);
    
    event_base_free(_base);

    _dispathThread->join();
    gbSAFE_DELETE(_dispathThread);
}

void gbSvrNet::_log_callback(int severity, const char* msg)
{
	gbLog::Instance().Log(gbString("severity:") + severity + ", msg:");
}

void gbSvrNet::_fatal_error_callback(int err)
{
	gbLog::Instance().Error(gbString("fatal error@") + err);
}

void gbSvrNet::_watchdog_ev_cb(evutil_socket_t fd, short what, void* arg)
{
    if(what & EV_READ)
    {
	sockaddr* srcSockAddr = new sockaddr;
	static socklen_t addrLen = sizeof(sockaddr);
	size_t len = recvfrom(fd, _recvBuffer, gb_UDP_MAX_PACKET_SIZE, 0, srcSockAddr, &addrLen);
	if(len != -1)
	{
	    gbLog::Instance().Log(gbString("_wd_ev_cb:") + (char*)_recvBuffer);
	}
	
//    event_base_loopexit(_base, NULL);
	//gbSvrNet::Instance().Shutdown();
	
    }
    else if(what & EV_WRITE)
    {
	gbLog::Instance().Log("_wd_ev_cb_write");
    }
}
void gbSvrNet::_ev_cb(evutil_socket_t fd, short what, void* arg)
{
    if(what & EV_READ)
    {
	sockaddr* srcSockAddr = new sockaddr;
	static socklen_t addrLen = sizeof(sockaddr);
	size_t len = recvfrom(fd, _recvBuffer, gb_UDP_MAX_PACKET_SIZE, 0, srcSockAddr, &addrLen);
	if(len != -1)
	{
	    unsigned char* rBuf = new unsigned char[len];
	    memcpy(rBuf, _recvBuffer, len);

	    gbUDPData *ud = new gbUDPData(rBuf, len, srcSockAddr, addrLen);

	    gbUDPDataHandler::Instance().Handle(ud);
	}
	else
	{
	    gbLog::Instance().Error((gbString)"recvfrom err@" + strerror(errno));
	    gbSAFE_DELETE(srcSockAddr);
	}

    }
    else if (what & EV_WRITE)
    {
	gbLog::Instance().Log("EV_WRITE");
    }
}

void gbSvrNet::_listener_cb(evconnlistener* listener, evutil_socket_t sock, sockaddr* addr, int socklen, void* ptr)
{
    event_base* base = evconnlistener_get_base(listener);
    if(base != nullptr)
    {
	event* ev = event_new(base, sock, EV_READ | EV_WRITE | EV_PERSIST | EV_ET, _ev_cb, NULL);
	event_add(ev, NULL);
    }
}

void gbSvrNet::_listener_error_cb(evconnlistener* listener, void* ptr)
{
    gbLog::Instance().Error("_listener_error_cb");
}
