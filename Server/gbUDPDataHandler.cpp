#include "gbUDPDataHandler.h"
#include "Log/gbLog.h"
#include "String/gbString.h"
#include "gbSvrLogic.h"
gbUDPActorMsg::gbUDPActorMsg(gbUDPData* data):
    _data(data),
    _processed(false)
{
    
}

gbUDPActorMsg::gbUDPActorMsg(const gbUDPActorMsg & msg):
    _data(msg._data),
    _processed(msg._processed)
{
    
}

gbUDPActorMsg::~gbUDPActorMsg()
{
    gbSAFE_DELETE(_data);
}
void gbUDPActorMsg::Process(const unsigned int actorIdx)
{
    // char type = *(char*)_data->Data();
    // if(type == 'X')
    // {
    // 	char* lua_cmd = (char*)(_data->Data() + 1);
    // 	gbLog::Instance().Log(gbString("processing data:") + lua_cmd);
    // 	lua_State* l = gbSvrLogic::Instance().GetLNASubState(actorIdx);
    // 	gbLuaCPP_dostring(l, lua_cmd);
    // }
    // else if(type == 'T')
    // {
	
    // }
    
    _data->Process(gbSvrLogic::Instance().GetLNASubState(actorIdx));
    _processed = true;    
}
gbUDPDataHandler::~gbUDPDataHandler()
{
    gbSAFE_DELETE(_handler);
    gbSAFE_DELETE(_dispatcher);
}

void gbUDPDataHandler::Initialize(unsigned int num)
{
    _handler = new Theron::Framework;

    _dispatcher = new gbActorDispatcher<gbUDPActorMsg*>(*_handler, num);
    _dispatcherAddr = _dispatcher->GetAddress();

    _rcvAddr = _rcv.GetAddress();
}


void gbUDPDataHandler::Handle(gbUDPData* udpData)
{
    gbUDPActorMsg* msg = new gbUDPActorMsg(udpData);
    _handler->Send(msg, _rcvAddr, _dispatcherAddr);
}
