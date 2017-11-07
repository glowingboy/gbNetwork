#include "gbIOEvent.h"
#include "gbIOEventHandler.h"

#include <arpa/inet.h>
#include "gbWatchdogComm.h"
gbIOEvent::~gbIOEvent()
{
    gbWatchdogIOTunnel* watchdog = _ioTunnelMgr.GetWatchdogClntIOTunnel();

    
    gbCommMsgString_UInt32s msg;
    msg.add_uint32vals(gb_WATCHDOGCOMM_CODE_SHUTDOWN);
    gbCommunicator* comm = watchdog->GetWatchdogComm();
    comm->SendTo(comm->GetAddr(), msg);

    _dispatchThread->join();

    gbSAFE_DELETE(_dispatchThread);

    for(std::list<event*>::iterator i = _lstEvs.begin(); i != _lstEvs.end(); i++)
    {
	event_free(*i);
    }

    if(_listener != nullptr)
	evconnlistener_free(_listener);
    evconnlistener_free(_watchdogListener);

    event_base_free(_base);


    
}
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
    

    sockaddr_in svrAddr;
    memset(&svrAddr, 0, sizeof(svrAddr));
    svrAddr.sin_family = AF_INET;
    svrAddr.sin_port = htons(port);

    //watchdog svr
    _watchdogSvrSocketFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if( _watchdogSvrSocketFd == -1)
    {
	gbLog::Instance().Error(gbString("_watchdogSvrSockFd create err@") + strerror(errno));
	return false;
    }

    svrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int optval = 1;
    if(setsockopt(_watchdogSvrSocketFd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)
    {
    	gbLog::Instance().Error(gbString("setsockopt SO_REUSEPORT err@") + strerror(errno));
    	return false;
    }

    if(bind(_watchdogSvrSocketFd, (sockaddr*)&svrAddr, sizeof(svrAddr)) == -1)
    {
    	gbLog::Instance().Error(gbString("bind err@") + strerror(errno));
    	return false;
    }
    
    _watchdogListener = evconnlistener_new(_base,_watchdog_listener_cb, this, LEV_OPT_CLOSE_ON_FREE, -1, _watchdogSvrSocketFd);
    evconnlistener_set_error_cb(_watchdogListener, _listener_error_cb);

    //watchdog client
    evutil_socket_t watchdogClntSocketFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(watchdogClntSocketFd == -1)
    {
	gbLog::Instance().Error(gbString("watchdogClntSockFd create err@") + strerror(errno));
	return false;
    }
    gbWatchdogIOTunnel* clntIOTunnel = new gbWatchdogIOTunnel(watchdogClntSocketFd);
    _ioTunnelMgr.SetWatchdogClntIOTunnel(clntIOTunnel);
//    gbIOTunnelMgr::Instance().AddIOTunnel(ioTunnel);

    Connect(watchdogClntSocketFd, "127.0.0.1", port);
    // event* ev = event_new(_base, watchdogClntSocketFd, EV_READ | EV_WRITE | EV_PERSIST | EV_ET, _ev_cb, NULL);
    // event_add(ev, NULL);
    // _lstEvs.push_back(ev);
    
    
    // if(connect(watchdogClntSocketFd, (sockaddr*)&svrAddr, sizeof(svrAddr)) == -1)
    // {
    // 	gbLog::Instance().Error(gbString("watchdogClntSockFd connect err@") + strerror(errno));
    // 	return false;
    // }
    

    //main svr
    _listener = nullptr;
    if(szLocalIP != nullptr)
    {
	_svrSocketFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if(_svrSocketFd == -1)
	{
	    gbLog::Instance().Error(gbString("sockfd create err@") + strerror(errno));
	    return false;
	}

	svrAddr.sin_addr.s_addr = inet_addr(szLocalIP);
	optval = 1;
	if(setsockopt(_svrSocketFd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)
	{
	    gbLog::Instance().Error(gbString("setsockopt SO_REUSEPORT err@") + strerror(errno));
	    return false;
	}

	if(bind(_svrSocketFd, (sockaddr*)&svrAddr, sizeof(svrAddr)) == -1)
	{
	    gbLog::Instance().Error(gbString("bind err@") + strerror(errno));
	    return false;
	}

	_listener = evconnlistener_new(_base,_listener_cb, this, LEV_OPT_CLOSE_ON_FREE, -1, _svrSocketFd);
	evconnlistener_set_error_cb(_listener, _listener_error_cb);
    }
    
    _dispatchThread = new std::thread([&]()
				     {
					 event_base_dispatch(_base);					 			       });
    
    return true;
}

void gbIOEvent::Connect(evutil_socket_t socket, const char* IP, unsigned short port)
{
    sockaddr_in svrAddr;
    memset(&svrAddr, 0, sizeof(svrAddr));
    svrAddr.sin_family = AF_INET;
    svrAddr.sin_port = htons(port);
    svrAddr.sin_addr.s_addr = inet_addr(IP);
    if(socket == -1)
    {
	gbLog::Instance().Error("gbIOEvent::Connect socket == -1");
	return;
    }
    event* ev = event_new(_base, socket, EV_READ | EV_WRITE | EV_PERSIST | EV_ET, _ev_cb, this);
    event_add(ev, NULL);
    _lstEvs.push_back(ev);
    
    if(connect(socket, (sockaddr*)&svrAddr, sizeof(svrAddr)) == -1)
    {
	if(errno != EINPROGRESS)
	    gbLog::Instance().Error(gbString("gbIOEvent::Connect connect err@") + strerror(errno));
    }
}
void gbIOEvent::Shutdown()
{
    event_base_loopexit(_base, NULL);
//    ::close(_sockfd);
//    event_free(_ev);

//    ::close(_watchdogSockfd);
//    event_free(_watchdogEv);
    


//    _dispatchThread->join();   

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
    gbIOEvent* ioEV = reinterpret_cast<gbIOEvent*>(arg);    
    if(what & EV_READ)
    {
	ioEV->_ioEventHandler.Handle(gb_IOEVENT_READABLE, ioEV->_ioTunnelMgr.GetIOTunnel(fd), nullptr);
    }
    else if (what & EV_WRITE)
    {
	ioEV->_ioEventHandler.Handle(gb_IOEVENT_WRITABLE, ioEV->_ioTunnelMgr.GetIOTunnel(fd), nullptr);
	gbLog::Instance().Log("EV_WRITE");
    }
}

void gbIOEvent::_listener_cb(evconnlistener* listener, evutil_socket_t sock, sockaddr* addr, int socklen, void* ptr)
{
    gbLog::Instance().Log("listener cb");
    gbIOEvent* ioEV = reinterpret_cast<gbIOEvent*>(ptr);
    
    event_base* base = evconnlistener_get_base(listener);
    if(base != nullptr)
    {
	event* ev = event_new(base, sock, EV_READ | EV_WRITE | EV_PERSIST | EV_ET, _ev_cb, ioEV);
	event_add(ev, NULL);
	ioEV->_lstEvs.push_back(ev);
    }
}

void gbIOEvent::_listener_error_cb(evconnlistener* listener, void* ptr)
{
    gbLog::Instance().Error("_listener_error_cb");
}

void gbIOEvent::_watchdog_ev_cb(evutil_socket_t fd, short what, void* arg)
{
    //read(internal affairs) need be handled in IOEvent thread, but not for write
    gbIOEvent* ioEV = reinterpret_cast<gbIOEvent*>(arg);
    if(what & EV_READ)
    {
	gbIOTunnel* tunnel = ioEV->_ioTunnelMgr.GetWatchdogSvrIOTunnel(fd);
	tunnel->Read();
	tunnel->ProcessRecvData();
    }
    else if( what & EV_WRITE)
    {
	ioEV->_ioEventHandler.Handle(gb_IOEVENT_WRITABLE, ioEV->_ioTunnelMgr.GetWatchdogSvrIOTunnel(fd), nullptr);
	gbLog::Instance().Log("EV_WRITE");
    }
}
void gbIOEvent::_watchdog_listener_cb(evconnlistener* listener, evutil_socket_t sock, sockaddr* addr, int socklen, void* ptr)
{
    gbLog::Instance().Log("listener cb");
    gbIOEvent* ioEV = reinterpret_cast<gbIOEvent*>(ptr);
    
    event_base* base = evconnlistener_get_base(listener);
    if(base != nullptr)
    {
	event* ev = event_new(base, sock, EV_READ | EV_WRITE | EV_PERSIST | EV_ET, _watchdog_ev_cb, ioEV);
	event_add(ev, NULL);
	ioEV->_lstEvs.push_back(ev);
    }

}
