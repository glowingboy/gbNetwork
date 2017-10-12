#pragma once
#include "../gbUDPData.h"
#include "gbCommon.h"
#include "gbActor.h"
class gbUDPActorMsg
{
public:
    gbUDPActorMsg(gbUDPData* data);
    gbUDPActorMsg(const gbUDPActorMsg & msg);
    ~gbUDPActorMsg();
    void Process(const unsigned int actorIdx);
    inline bool IsProcessed() const {return _processed;}
private:
    gbUDPData* _data;
    bool _processed;
};
class gbUDPDataHandler
{
    SingletonDeclare(gbUDPDataHandler);
public:
    void Initialize(unsigned int num);
    void Handle(gbUDPData* udpData);
    ~gbUDPDataHandler();
private:
    Theron::Framework* _handler;
    gbActorDispatcher<gbUDPActorMsg*>* _dispatcher;
    Theron::Address _dispatcherAddr;
    Theron::Receiver _rcv;
    Theron::Address _rcvAddr;
};


