#include "gbClientCore.h"

#include "gbUtils/include/gbString.h"
#ifdef _MSC_VER
#include<Windows.h>
#endif

#ifdef __GNUC__
#include <arpa/inet.h>
#endif

#include "gbUtils/include/gbLog.h"

bool gbClientCore::_is_little_endian;
std::mutex gbClientCore::_sendPkgMutex;
std::mutex gbClientCore::_writableMutex;

std::condition_variable gbClientCore::_sendThread_cv;
std::priority_queue<gbSendPkg*> gbClientCore::_sendPkgs;
bool gbClientCore::_writable = true;
evutil_socket_t gbClientCore::_sockfd;
event* gbClientCore::_ev;
addrinfo* gbClientCore::_svrInfo;
unsigned char gbClientCore::_recvBuffer[gb_UDP_MAX_PACKET_SIZE];

bool gbClientCore::Initialize()
{
    int t = 1;
    _is_little_endian = ((char*)&t)[0];

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

    return true;

}

gbClientCore::gbClientCore()
{
#ifdef WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) 
    {
	gbLog::Instance().Error(gbString("WSAStartup failed with error: ") + err);
	throw std::exception("WSAStartup error");
    }
#endif
    event_set_log_callback(_log_callback);

#ifdef _DEBUG
    event_enable_debug_mode();
#endif

    event_set_fatal_callback(_fatal_error_callback);

    //todo, Memory management

    _ev_base = event_base_new();

    //const char** methods = event_get_supported_methods();
    //int i = 0;
    //while (methods[i] != nullptr)
    //{
    //	std::cout << "supported method:" << methods[i++] << std::endl;
    //}

	
}

gbClientCore::~gbClientCore()
{
    event_base_free(_ev_base);
    gbSAFE_DELETE(_loop_thread);
    gbSAFE_DELETE(_dedicated_send_thread);
}

bool gbClientCore::Connect(const char* ip, const char* port)
{
#ifdef __GNUC__
    addrinfo hint;
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_DGRAM;
    hint.ai_protocol = IPPROTO_UDP;
    int ret = ::getaddrinfo(ip, port, &hint, &_svrInfo);
    if(ret != 0)
    {
	
	gbLog::Instance().Error((gbString)"getaddrinfo err@" + strerror(ret == EAI_SYSTEM ? errno : ret));
	return false;
    }
    _sockfd = socket(_svrInfo->ai_family, _svrInfo->ai_socktype, _svrInfo->ai_protocol);
    if(_sockfd == -1)
    {
	gbLog::Instance().Error(gbString("sockfd create err@") + strerror(errno));
	return false;
    }

    _ev = event_new(_ev_base, _sockfd, EV_READ | EV_WRITE, _ev_cb, NULL);
    event_add(_ev, NULL);

    _dedicated_send_thread = new std::thread(_send_thread);
#elif _MSC_VER
    
    _bev = bufferevent_socket_new(_ev_base, -1, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
    bufferevent_setcb(_bev, _bufferevent_readcb, _bufferevent_writecb, _bufferevent_cb, this);
    bufferevent_enable(_bev, EV_READ | EV_WRITE | EV_PERSIST);

    sockaddr_in sin;
    memset(&sin, 0, sizeof(sockaddr_in));
    sin.sin_family = AF_INET;

    if (!inet_pton(AF_INET, ip, &(sin.sin_addr)))
    {
	gbLog::Instance().Error(gbString("invalid ip format@") + ip);
	return false;
    }

    //sin.sin_addr.s_addr = htonl(0x7f000001);
    sin.sin_port = htons(port);

    if (bufferevent_socket_connect(_bev, (sockaddr*)&sin, sizeof(sockaddr_in)) < 0)
    {
	gbLog::Instance().Error("bufferevent_socket_connect");
	return false;
    }
#endif
    _loop_thread = new std::thread([this]()
				   {
				       try
				       {
					   event_base_dispatch(_ev_base);
				       }
				       catch (std::exception& e)
				       {
					   gbLog::Instance().Error(e.what());
				       }
				   });
    return true;
}


void gbClientCore::_ev_cb(evutil_socket_t fd, short what, void* arg)
{
    if(what & EV_READ)
    {
	size_t len = recvfrom(fd, _recvBuffer, 512, 0, NULL, NULL);
	if(len != -1)
	{
	    unsigned char* rBuf = new unsigned char[len];
	    //release after decoded
	    memcpy(rBuf, _recvBuffer, len);
	
//	    gbUDPDataMgr::Instance().Push(rBuf, len);
	}
	else
	    gbLog::Instance().Error((gbString)"recvfrom err@" + strerror(errno));
    }
    else if (what & EV_WRITE)
    {
	{
	    std::lock_guard<std::mutex> lck(_writableMutex);
	    _writable = true;
	    _sendThread_cv.notify_one();
	}
	
    }
}

bool gbClientCore::Send(const unsigned char* msg, const size_t size)
{
    //if (_is_little_endian)
    //	_bytes_reverse(const_cast<char*>((const char*)msg), size);

    //return !bufferevent_write(_bev, msg, size);
    {
	std::lock_guard<std::mutex> lck(_sendPkgMutex);
	if (_sendPkgs.size() < 1024)
	{
	    _sendPkgs.push(new gbSendPkg(msg, size, true, gbSendPkg::Priority::mid));
	    _sendThread_cv.notify_one();
	    return true;
	}
	else
	{
	    _sendThread_cv.notify_one();
	    gbLog::Instance().Error(gbString("_sendPkgs.size() > 1024 SLOW DOWN!!! size@") + (unsigned int)_sendPkgs.size());
	    return false;
	}

	//event_add
    }

}

bool gbClientCore::Send(gbSendPkg* pkg)
{
    bool ret = false;
    {
	std::lock_guard<std::mutex> lck(_sendPkgMutex);
	if (_sendPkgs.size() < 1024)
	{
	    _sendPkgs.push(pkg);
	    ret = true;
	}
	else
	{
	    gbLog::Instance().Error(gbString("_sendPkgs.size() > 1024 SLOW DOWN!!! size@") + (unsigned int)_sendPkgs.size());
	    ret = false;
	}
    }
    _sendThread_cv.notify_one();
    return ret;
}


// void gbClientCore::_bufferevent_writecb(struct bufferevent *bev, void *ctx)
// {
//     {
// 	std::lock_guard<std::mutex> lck(_writableMutex);
// 	_writable = true;
// 	_sendThread_cv.notify_one();
//     }
// }

void gbClientCore::_send_thread()
{
    while (true)
    {
	try
	{
	    std::unique_lock<std::mutex> lck(_sendPkgMutex);	    
	    
	    if (_sendPkgs.size() > 0 && _writable)
	    {
		gbSendPkg pkg(std::move(*_sendPkgs.top()));
		_sendPkgs.pop();
		lck.unlock();

		{
		    std::lock_guard<std::mutex> lck(_writableMutex);
		    //set writable false, before write, in case of write_cb is called after write
		    _writable = false;
		}

		int len = ::sendto(_sockfd, pkg.data, pkg.GetSize(), 0, _svrInfo->ai_addr, _svrInfo->ai_addrlen);
		if(len != -1)
		{
		    {
			std::lock_guard<std::mutex> lck(_writableMutex);
			//set writable back to true
			_writable = true;
		    }
		}
		else
		{
		    gbLog::Instance().Error(gbString("sendto size@") + (unsigned int)pkg.GetSize() + "err@" + strerror(errno));
		    {
			std::lock_guard<std::mutex> lck(_writableMutex);
			//if writable then, write_cb has been called
			if (!_writable)
			    _writable = false;
		    }
		}
		    
		// if (bufferevent_write(bev, pkg.data, pkg.GetSize()) == -1)
		// {
		// 	gbLog::Instance().Error(gbString("bufferevent_write size@") + pkg.GetSize());
		// 	{
		// 	    std::lock_guard<std::mutex> lck(_writableMutex);
		// 	    //if writable then, write_cb has been called
		// 	    if (!_writable)
		// 		_writable = false;
		// 	}
		// }
		// else
		// {
		// 	{
		// 	    std::lock_guard<std::mutex> lck(_writableMutex);
		// 	    //set writable back to true
		// 	    _writable = true;
		// 	}
		// }
	    }
	    else
		_sendThread_cv.wait(lck);
	}
	catch(std::exception& e)
	{
	    gbLog::Instance().Error(gbString("lck err@") + e.what());
	    return;
	}

    }
}
void gbClientCore::_log_callback(int severity, const char* msg)
{
    gbLog::Instance().Log(gbString("severity:") + severity + ", msg:");
}

void gbClientCore::_fatal_error_callback(int err)
{
    gbLog::Instance().Error(gbString("fatal error@") + err);
}


// void gbClientCore::_bufferevent_readcb(bufferevent* bev, void *ptr)
// {
//     //std::cout << "data comming" << std::endl;
//     //char buf[1024];
//     //int n;
//     //evbuffer* input = bufferevent_get_input(bev);
//     //while ((n = evbuffer_remove(input, buf, sizeof(buf))) > 0) {
//     //	fwrite(buf, 1, n, stdout);
//     const size_t length = evbuffer_get_length(bev->input);

//     //release after decoded
//     unsigned char* buffer = new unsigned char[length];
//     bufferevent_read(bev, buffer, length);
//     gbRawDataMgr::Instance().Push(buffer, length);

//     //notify appPkgMgr to decode

//     return;
//     char tmp[1024] = { '\0' };
//     size_t len = bufferevent_read(bev, tmp, 1024);
//     //if (_is_little_endian)
//     //	_bytes_reverse(tmp, len);
//     len = len == 1024 ? 1023 : len;
//     tmp[len] = '\0';
//     std::cout << "received:" << tmp << std::endl;

// }
// void gbClientCore::_bufferevent_cb(bufferevent* bev, short events, void *ptr)
// {
//     gbClientCore* core = static_cast<gbClientCore*>(ptr);

//     if (events & BEV_EVENT_CONNECTED)
//     {
// 	gbLog::Instance().Log("connected");

// 	//todo, create another eventbase?
// //	auto f = std::bind(_send_thread, bev);
// 	core->_dedicated_send_thread = new std::thread(_send_thread);
//     }

//     else if (events & BEV_EVENT_TIMEOUT)
// 	std::cout << "timeout";
//     else if (events & BEV_EVENT_ERROR)
// 	std::cout << "error@" << evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR());
// }
