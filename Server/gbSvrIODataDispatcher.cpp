#include "gbSvrIODataDispatcher.h"


void gbSvrIODataDispatcher::Msg::Process(const unsigned int actorIdx)
{
    _ioTunnel->ProcessRecvData();
}


gbSvrIODataDispatcher::~gbSvrIODataDispatcher()
{
    gbSAFE_DELETE(_framework);
    gbSAFE_DELETE(_dispatcher);
}

void gbSvrIODataDispatcher::Initialize(unsigned int num)
{
    _framework = new Theron::Framework;

    _dispatcher = new gbSerializeActorDispatcher<Msg, gb_socket_t>(*_framework, num);
    _dispatcherAddr = _dispatcher->GetAddress();

    _rcvAddr = _rcv.GetAddress();
}


void gbSvrIODataDispatcher::Dispatch(gbIOTunnel* ioTunnel)
{
    std::lock_guard<std::mutex> lck(_mtx);
    _framework->Send(Msg(ioTunnel), _rcvAddr, _dispatcherAddr);
}



