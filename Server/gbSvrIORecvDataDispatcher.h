#pragma once

#include "gbCommon.h"
#include "gbActor.h"
#include <mutex>
#include "../gbIOTunnel.h"
class gbSvrIORecvDataDispatcher
{
private:
    class Msg
    {
    public:
	inline Msg(gbIOTunnel* ioTunnel):
	    _ioTunnel(ioTunnel)
	    {}
	inline Msg(const Msg& other):
	    _ioTunnel(other._ioTunnel)
	    {}
	void Process(const unsigned int actorIdx);
	inline void SetProcessed() { _processed = true; }
	inline bool IsProcessed() const { return _processed; }
	inline gb_socket_t GetKey() const { return _ioTunnel->GetSocket(); }
    private:
	gbIOTunnel* _ioTunnel;
	bool _processed;
    };
    SingletonDeclare(gbSvrIORecvDataDispatcher);
public:
    void Initialize(unsigned int num);
    void Dispatch(gbIOTunnel* ioTunnel);
    ~gbSvrIORecvDataDispatcher();

private:
    Theron::Framework* _framework;
    gbSerializeActorDispatcher<Msg, gb_socket_t>* _dispatcher;
    Theron::Address _dispatcherAddr;
    Theron::Receiver _rcv;
    Theron::Address _rcvAddr;
    std::mutex _mtx;

};
