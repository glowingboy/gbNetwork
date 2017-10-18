#include "gbNWClient.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Log/gbLog.h"
#include "String/gbString.h"

bool gbNWClient::Initialize(const unsigned short watchdogPort)
{
    _socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if(_socket == -1)
    {
	gbLog::Instance().Error(gbString("sockfd create err@") + strerror(errno));
	return false;
    }

#ifdef _DEBUG
    event_enable_debug_mode();
#endif
    
    event_set_log_callback(_log_callback);
    event_set_fatal_callback(_fatal_error_callback);

    event_config* cfg = event_config_new();
    if(cfg == nullptr)
	return false;
    event_config_set_flag(cfg, EVENT_BASE_FLAG_NOLOCK);
    _evBase = event_base_new_with_config(cfg);
    event_config_free(cfg);
    
//    _base = event_base_new();
    if (_evBase == nullptr)
	return false;

    _ev = event_new(_evBase, _socket, EV_READ | EV_WRITE | EV_PERSIST | EV_ET, _ev_cb, this);
    if(_ev == nullptr)
	return false;
    
    event_add(_ev, NULL);

    //watchdog
    //wd svr
    _watchdogSvrSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    sockaddr_in watchdogAddr;
    memset(&watchdogAddr, 0, sizeof(watchdogAddr));
    watchdogAddr.sin_family = AF_INET;
    watchdogAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    watchdogAddr.sin_port = htons(watchdogPort);

    if(bind(_watchdogSvrSocket, (sockaddr*)&watchdogAddr, sizeof(watchdogAddr)) == -1)
    {
	gbLog::Instance().Error("bind watchdogSocket err");
	return false;
    }
    
    _watchdogListener = evconnlistener_new(_evBase, _watchdogListener_cb, this, LEV_OPT_CLOSE_ON_FREE, -1, _watchdogSvrSocket);

    evconnlistener_set_error_cb(_watchdogListener, _watchdogListener_error_cb);

    //wd clnt
    _watchdogClntSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    _watchdogClnt_ev = event_new(_evBase, _watchdogClntSocket, EV_READ | EV_WRITE | EV_PERSIST | EV_ET, _ev_cb, this);
    if(_watchdogClnt_ev == nullptr)
	return false;
    
    event_add(_watchdogClnt_ev, NULL);
    
    
    connect(_watchdogClntSocket, (struct sockaddr*)&watchdogAddr, sizeof(watchdogAddr));

    _dispatchThread = new std::thread([&]()
				      {
					  event_base_dispatch(_evBase);
				      }
	);

    return true;
}

void gbNWClient::Connect(const char* szIP, const unsigned short port)
{
    sockaddr_in svrAddr;
    memset(&svrAddr, 0, sizeof(svrAddr));
    svrAddr.sin_family = AF_INET;
    svrAddr.sin_addr.s_addr = inet_addr(szIP);
    svrAddr.sin_port = htons(port);

    connect(_socket, (struct sockaddr*)&svrAddr, sizeof(svrAddr));

}

void gbNWClient::Close()
{
    if(event_base_loopexit(_evBase, NULL) == -1)
	gbLog::Instance().Error("event_base_loopexit error");
    ::close(_socket);
    event_base_free(_evBase);
    event_free(_ev);
    
    gbLog::Instance().Log("c_4");
    _dispatchThread->join();
    gbLog::Instance().Log("c_5");
    
    gbSAFE_DELETE(_dispatchThread);
}
void gbNWClient::_log_callback(int severity, const char* msg)
{
	gbLog::Instance().Log(gbString("severity:") + severity + ", msg:");
}

void gbNWClient::_fatal_error_callback(int err)
{
	gbLog::Instance().Error(gbString("fatal error@") + err);
}

void gbNWClient::_ev_cb(evutil_socket_t fd, short what, void* arg)
{
    gbNWClient* clnt = reinterpret_cast<gbNWClient*>(arg);
    
    if(what & EV_READ)
    {
	gbLog::Instance().Log("EV_READ");	
    }
    else if( what & EV_WRITE)
    {
	gbLog::Instance().Log("EV_WRITE");
    }
    else if( what & EV_PERSIST)
    {
	gbLog::Instance().Log("EV_PERSIST");
    }
}


void gbNWClient::_watchdogListener_cb(evconnlistener* listener, evutil_socket_t sock, sockaddr* addr, int socklen, void* ptr)
{
    gbNWClient* clnt = reinterpret_cast<gbNWClient*>(ptr);
    
    event_base* base = evconnlistener_get_base(listener);
    if(base != nullptr)
    {
	clnt->_watchdogSvr_ev = event_new(base, sock, EV_READ | EV_WRITE | EV_PERSIST | EV_ET, _ev_cb, clnt);
	event_add(clnt->_watchdogSvr_ev, NULL);
    }
}

void gbNWClient::_watchdogListener_error_cb(evconnlistener* listener, void* ptr)
{
    gbNWClient* clnt = reinterpret_cast<gbNWClient*>(ptr);
    
    gbLog::Instance().Error("_listener_error_cb");
}
