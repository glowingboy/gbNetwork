#include "gbSvrIORecvDataDispatcher.h"


void gbSvrIORecvDataDispatcher::Msg::Process(const unsigned int actorIdx)
{
    _ioTunnel->ProcessRecvData();
}

void gbSvrIORecvDataDispatcher::Initialize(unsigned int num)
{
    _framework = new Theron::Framework;

    _dispatcher = new gbSerializeActorDispatcher<Msg, gb_socket_t>(*_framework, num);
    _dispatcherAddr = _dispatcher->GetAddress();

    _rcvAddr = _rcv.GetAddress();
}


void gbSvrIORecvDataDispatcher::Dispatch(gbIOTunnel* ioTunnel)
{
    std::lock_guard<std::mutex> lck(_mtx);
    _framework->Send(Msg(ioTunnel), _rcvAddr, _dispatcherAddr);
}



