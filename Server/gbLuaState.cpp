#include "gbLuaState.h"


lua_State* gbLuaStateMgr::GetAccInfoState()
{
    lua_State* ls = lua_newthread(_accInfoState);
    return ls;
}
