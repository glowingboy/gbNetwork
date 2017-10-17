#include "gbNWClient.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Log/gbLog.h"
#include "String/gbString.h"

bool gbNWClient::Initialize()
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

    _ev = event_new(_evBase, _socket, EV_READ | EV_WRITE | EV_PERSIST | EV_ET, _ev_cb, NULL);
    if(_ev == nullptr)
	return false;
    
    event_add(_ev, NULL);
    
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

    _dispatchThread = new std::thread([&]()
				      {
					  event_base_dispatch(_evBase);
				      }
	);
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
    if(what & EV_READ)
    {
	gbLog::Instance().Log("EV_READ");	
    }
    else if( what & EV_WRITE)
    {
	gbLog::Instance().Log("EV_WRITE");
    }
}
