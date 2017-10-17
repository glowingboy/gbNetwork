#include "gbTCPPkgHandler.h"
#include "gbSvrNet.h"
#include "gbSvrLogic.h"
gbTCPPkgActorMsg::gbTCPPkgActorMsg(gbTCPRawData* data):
    _data(data),
    _processed(false)
{}

gbTCPPkgActorMsg::gbTCPPkgActorMsg(const gbTCPPkgActorMsg & other):
    _data(other._data),
    _processed(other._processed)
{}



gbTCPPkgActorMsg::~gbTCPPkgActorMsg()
{
    gbSAFE_DELETE(_data);
}

void gbTCPPkgActorMsg::Process(const unsigned int actorIdx)
{
    //lock for processing datas from same socket serially
    //possible issue: if there are too many datas from same socket are loaded to different actors,
    //then it may cause too many actors to be busy at the same time, and corrupt concurrency finally.
    //solution: optimize dispatching method(the same socket data will be dispatched to the same actor when there is a same socket already occupying an actor),
    //using gbSerializeactordispatcher, then all datas with same socket will be handled sequencially, and there is no more need lock operation
    
//    std::lock_guard<std::mutex> lck(_data->socketData->GetMtx());
    
    unsigned int lenData = _data->length;
    unsigned char* data = _data->data;
    std::vector<unsigned char>& remainder = _data->socketData->GetRemainderData();

    if(remainder.size() != 0)
    {
	remainder.insert(remainder.end(), data, data + lenData);
	data = remainder.data();
	lenData = remainder.size();
    }
    
    for(;;)
    {
	tcpPkgLen len = *(tcpPkgLen*)data;
	static char sizeofTcpPkgLen = sizeof(tcpPkgLen);
	if(lenData >= (len + sizeofTcpPkgLen))
	{
	    gbAppPkg::Handle(data, len, _data->socketData->GetSocket(), gbSvrLogic::Instance().GetLNASubState(actorIdx));
	}
    }

}

gbTCPPkgHandler::~gbTCPPkgHandler()
{
    gbSAFE_DELETE(_handler);
    gbSAFE_DELETE(_dispatcher);
}

void gbTCPPkgHandler::Initialize(unsigned int num)
{
    _handler = new Theron::Framework;

    _dispatcher = new gbSerializeActorDispatcher<gbTCPPkgActorMsg*, gb_socket_t>(*_handler, num);
    _dispatcherAddr = _dispatcher->GetAddress();

    _rcvAddr = _rcv.GetAddress();
}


void gbTCPPkgHandler::Handle(gbTCPRawData* rData)
{
    gbTCPPkgActorMsg* msg = new gbTCPPkgActorMsg(rData);
    _handler->Send(msg, _rcvAddr, _dispatcherAddr);
}
