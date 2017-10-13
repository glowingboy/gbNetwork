#include "gbSvrLogic.h"
#include "Log/gbLog.h"
#include "FileSystem/gbFileSystem.h"
#include "Data/gbAccInfo.h"
bool gbSvrLogic::Start()
{
    //lua network api(LNA)
    lua_State* lnaState = luaL_newstate();
    if(lnaState == nullptr)
    {
	gbLog::Instance().Log("luaL_newstate nullptr");
	return false;
    }
    luaL_openlibs(lnaState);

    //register lua api
    gbAccInfo::gb_LC_Reg(lnaState);
    gbAccInfoAccesser::gb_LC_Reg(lnaState, "gbLLA");
    

    gbFileSystem::Instance().GetWorkPath(_workPath);
    gbLuaCPP_appendPackagePath(lnaState, _workPath + "../Script/LNA/");
    gbLuaCPP_appendPackagePath(lnaState, _workPath + "../Script/LLA/");
    gbLuaCPP_dofile(lnaState, _workPath + "../Script/LNA/gbLNA.lua");
    
    
    lua_State* lnaSubState[16] = {nullptr};
    for(int i = 0; i < 16; i++)
    {
	lnaSubState[i] = lua_newthread(lnaState);
    }

    

}
