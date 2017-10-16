#pragma once
#include "../gbUDPData.h"
#include "gbCommon.h"
#include "LuaCPP/gbLuaCPP.h"

class gbSvrUDPData:public gbUDPData
{
public:
    void Porcess(lua_State* l);
    void Send();
};

class gbSvrUDPDataSendMgr
{
    SingletonDeclare(gbSvrUDPDataSendMgr);
public:
    
}
