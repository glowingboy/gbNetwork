#pragma once

#include "gbUtils/include/gbCommon.h"
#include "gbActor.h"
#include <mutex>
#include "../gbIOTunnel.h"
class gbIOEventHandler;
class gbSvrIORecvDataDispatcher
{
    friend class gbIOEventHandler;
private:
    class Msg
    {
    public:
	inline Msg(gbIOTunnel* ioTunnel):
	    _ioTunnel(ioTunnel),
	    _processed(false)
	    {}
	inline Msg(const Msg& other):
	    _ioTunnel(other._ioTunnel),
	    _processed(other._processed)
	    {}
	void Process(const unsigned int actorIdx);
	inline void SetProcessed() { _processed = true; }
	inline bool IsProcessed() const { return _processed; }
	inline gb_socket_t GetKey() const { return _ioTunnel->GetSocket(); }
    private:
	gbIOTunnel* _ioTunnel;
	bool _processed;
    };
//    SingletonDeclare(gbSvrIORecvDataDispatcher);
    inline gbSvrIORecvDataDispatcher(){}
public:
    void Initialize(unsigned int num);
    void Dispatch(gbIOTunnel* ioTunnel);
    inline ~gbSvrIORecvDataDispatcher()
	{
	    gbSAFE_DELETE(_dispatcher);
	    gbSAFE_DELETE(_framework);
	}

private:
    Theron::Framework* _framework;
    gbSerializeActorDispatcher<Msg, gb_socket_t>* _dispatcher;
    Theron::Address _dispatcherAddr;
    Theron::Receiver _rcv;
    Theron::Address _rcvAddr;
    std::mutex _mtx;

};
