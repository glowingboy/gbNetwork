#pragma once
#include "gbCommon.h"
#include "LuaCPP/gbLuaCPP.h"
#include <String/gbString.h>

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
//    void EventHandle(const gb_socket_t socket);
private:
    gbString _workPath;

    lua_State* _lnaState;
    lua_State* _lnaSubState[gb_SVR_LOGIC_UDP_HANDLER_ACTOR_NUM];
};
