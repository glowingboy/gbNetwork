#pragma once
#include "gbCommon.h"
#include "LuaCPP/gbLuaCPP.h"
class gbLuaState
{
    
};

class gbLuaStateMgr
{
    SingletonDeclare(gbLuaStateMgr);
public:
    inline void SetAccInfoState(lua_State* l){ _accInfoState = l; }
    lua_State* GetAccInfoState();
    
private:
    lua_State* _accInfoState;
};
