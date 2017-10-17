#pragma once
#include "gbCommon.h"
#include <queue>
#include <vector>
#include "../gbAppPkg.h"
#include "Theron/Theron.h"
#include "gbActor.h"
#include "../gbTCPPkg.h"
#include "gbSvrNet.h"


struct gbTCPRawData
{
    inline ~gbTCPRawData()
	{
	    gbSAFE_DELETE(data);
	}
    unsigned char* data;
    unsigned int length;
    gbTCPSocketData* socketData;
};


typedef unsigned int tcpPkgLen;


class gbTCPPkgActorMsg
{
public:
    gbTCPPkgActorMsg(gbTCPRawData* data);
    gbTCPPkgActorMsg(const gbTCPPkgActorMsg & other);
    ~gbTCPPkgActorMsg();
    void Process(const unsigned int actorIdx);
    inline bool IsProcessed() const {return _processed;}
    inline gb_socket_t GetKey()const { return _data->socketData->GetSocket(); }
private:
    gbTCPRawData* _data;
    bool _processed;
};

class gbTCPPkgHandler
{
    SingletonDeclare(gbTCPPkgHandler);
public:
    void Initialize(unsigned int num);
    void Handle(gbTCPRawData* rData);
    ~gbTCPPkgHandler();
private:
    Theron::Framework* _handler;
    gbSerializeActorDispatcher<gbTCPPkgActorMsg*, gb_socket_t>* _dispatcher;
    Theron::Address _dispatcherAddr;
    Theron::Receiver _rcv;
    Theron::Address _rcvAddr;
};


