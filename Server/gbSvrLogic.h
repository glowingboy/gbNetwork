#pragma once
#include "gbUtils/include/gbCommon.h"
#include "gbUtils/include/gbLuaCPP.h"
#include <gbUtils/include/gbString.h>

#include "../gbType.h"

#define gb_SVR_LOGIC_UDP_HANDLER_ACTOR_NUM 16

class gbSvrLogic
{
SingletonDeclare(gbSvrLogic);
public:
bool Start();
inline lua_State* GetLNASubState(const unsigned int idx)
{
if(idx < gb_SVR_LOGIC_UDP_HANDLER_ACTOR_NUM)
    return _lnaSubState[idx];
else
    return nullptr;
}
private:
gbString _workPath;

lua_State* _lnaState;
lua_State* _lnaSubState[gb_SVR_LOGIC_UDP_HANDLER_ACTOR_NUM];
};
