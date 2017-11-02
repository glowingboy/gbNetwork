#include "gbIOEvent.h"
#include "gbIOEventHandler.h"

bool gbIOEvent::Start(const char* szLocalIP, const unsigned short port)
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
    
    _svrSocketFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(_svrSocketFd == -1)
    {
	gbLog::Instance().Error(gbString("sockfd create err@") + strerror(errno));
	return false;
    }

    sockaddr_in svrAddr;
    memset(&svrAddr, 0, sizeof(svrAddr));
    svrAddr.sin_family = AF_INET;
    svrAddr.sin_port = htons(port);
    
    //svrAddr.sin_addr.s_addr = inet_addr(szLocalIP);//todo
    svrAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    int optval = 1;
    if(setsockopt(_svrSocketFd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)
    {
    	gbLog::Instance().Error(gbString("setsockopt SO_REUSEPORT err@") + strerror(errno));
    	return false;
    }

    // if(fcntl(_svrSocketFd, F_SETFL, O_NONBLOCK) == -1)
    // {
    // 	gbLog::Instance().Error(gbString("fcntl err@") + strerror(errno));
    // 	return false;
    // }

    if(bind(_svrSocketFd, (sockaddr*)&svrAddr, sizeof(svrAddr)) == -1)
    {
    	gbLog::Instance().Error(gbString("bind err@") + strerror(errno));
    	return false;
    }

    _listener = evconnlistener_new(_base,_listener_cb, this, LEV_OPT_CLOSE_ON_FREE, -1, _svrSocketFd);
    evconnlistener_set_error_cb(_listener, _listener_error_cb);
    
    //watchdog socket todo
    // _watchdogSockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    // if(_watchdogSvrSocketFd == -1)
    // {
    // 	gbLog::Instance().Error(gbString("watchdogSockfd create err@") + strerror(errno));
    // 	return false;
    // }

    // if(setsockopt(_watchdogSvrSocketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    // {
    // 	gbLog::Instance().Error(gbString("setsockopt SO_REUSEADDR err@") + strerror(errno));
    // 	return false;
    // }

    // if(setsockopt(_watchdogSvrSocketFd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)
    // {
    // 	gbLog::Instance().Error(gbString("setsockopt SO_REUSEPORT err@") + strerror(errno));
    // 	return false;
    // }

    // //only receive from loopback interface
    // svrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // if(bind(_watchdogSvrSocketFd, (sockaddr*)&svrAddr, sizeof(svrAddr)) == -1)
    // {
    // 	gbLog::Instance().Error(gbString("bind err@") + strerror(errno));
    // 	return false;
    // }
    
    // _watchdogEv = event_new(_base, _watchdogSvrSocketFd, EV_READ | EV_WRITE | EV_PERSIST | EV_ET, _watchdog_ev_cb, NULL);
    // event_add(_watchdogEv, NULL);

    _dispathThread = new std::thread([&]()
				     {
					 event_base_dispatch(_base);					 			       });
    
    return true;
}


void gbIOEvent::Shutdown()
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

void gbIOEvent::_log_callback(int severity, const char* msg)
{
	gbLog::Instance().Log(gbString("severity:") + severity + ", msg:");
}

void gbIOEvent::_fatal_error_callback(int err)
{
	gbLog::Instance().Error(gbString("fatal error@") + err);
}

void gbIOEvent::_ev_cb(evutil_socket_t fd, short what, void* arg)
{
    if(what & EV_READ)
    {
	gbNWMessageDispatcher::Instance().Dispatch(gbNWMessageType::READABLE, gbSocketDataMgr::Instance().GetSocketData(fd), nullptr);
    }
    else if (what & EV_WRITE)
    {
	gbLog::Instance().Log("EV_WRITE");
    }
}

void gbIOEvent::_listener_cb(evconnlistener* listener, evutil_socket_t sock, sockaddr* addr, int socklen, void* ptr)
{
    gbLog::Instance().Log("listener cb");
    gbIOEvent* evMachine = reinterpret_cast<gbIOEvent*>(ptr);
    
    event_base* base = evconnlistener_get_base(listener);
    if(base != nullptr)
    {
	event* ev = event_new(base, sock, EV_READ | EV_WRITE | EV_PERSIST | EV_ET, _ev_cb, NULL);
	event_add(ev, NULL);
	evMachine->_lstEvs.push_back(ev);
	
	gbNWMessageDispatcher::Instance().Dispatch(gbNWMessageType::CONNECTED, gbSocketDataMgr::Instance().GetSocketData(sock), nullptr);
    }
}

void gbIOEvent::_listener_error_cb(evconnlistener* listener, void* ptr)
{
    gbLog::Instance().Error("_listener_error_cb");
}

