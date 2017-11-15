#pragma once

#include "gbUtils/include/gbCommon.h"
#include "gbType.h"

#ifdef gb_SVR
#include "Server/gbSvrIOEventDispatcher.h"
#include "Server/gbSvrIORecvDataDispatcher.h"
#elif gb_CLNT
#include "Client/gbClntIOEventHandler.h"
#include "Client/gbClntIORecvDataHandler.h"
#endif

#define gb_IOEVENT_READABLE 0x2
#define gb_IOEVENT_WRITABLE 0x4

class gbIOEvent;
class gbIOTunnel;
class gbIOEventHandler
{
//    SingletonDeclare(gbIOEventHandler);
    friend class gbIOEvent;
    inline gbIOEventHandler()
	{
#ifdef gb_SVR
	    _svrDispatcher.Initialize(16);
	    _svrRecvDataDispatcher.Initialize(16);
#endif
	}
public:
    void Handle(const unsigned char ioEventType, gbIOTunnel* ioTunnel, gb_array<unsigned char>* sendData = nullptr);
    
#ifdef gb_SVR
    gbSvrIOEventDispatcher& GetSvrIOEventDispatcher(){ return _svrDispatcher; }
    gbSvrIORecvDataDispatcher& GetSvrIORecvDataDispatcher(){return _svrRecvDataDispatcher; }
#elif gb_CLNT
    gbClntIOEventHandler& GetClntIOEventHandler(){ return _clntHandler; }
    gbClntIORecvDataHandler& GetClntIORecvDataHandler(){ return _clntRecvDataHandler; }
#endif
    
private:
#ifdef gb_SVR
    gbSvrIOEventDispatcher _svrDispatcher;
    gbSvrIORecvDataDispatcher _svrRecvDataDispatcher;
#elif gb_CLNT
    gbClntIOEventHandler _clntHandler;
    gbClntIORecvDataHandler _clntRecvDataHandler;
#endif
    
};
