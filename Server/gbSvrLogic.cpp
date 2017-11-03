#include "gbSvrLogic.h"
#include "Log/gbLog.h"
#include "FileSystem/gbFileSystem.h"
#include "Data/gbAccInfo.h"
#include "gbSvrIOEventDispatcher.h"
#include "gbSvrIODataDispatcher.h"
bool gbSvrLogic::Start()
{
    //lua network api(LNA)
    _lnaState = luaL_newstate();
    if(_lnaState == nullptr)
    {
	gbLog::Instance().Log("luaL_newstate nullptr");
	return false;
    }
    luaL_openlibs(_lnaState);

    //register lua api
    gbAccInfo::gb_LC_Reg(_lnaState);
    gbAccInfoAccesser::gb_LC_Reg(_lnaState);
    
    gbFileSystem::Instance().GetWorkPath(_workPath);
    gbLuaCPP_appendPackagePath(_lnaState, _workPath + "../Script/LNA");
    gbLuaCPP_appendPackagePath(_lnaState, _workPath + "../Script/LLA");
    gbLuaCPP_appendPackagePath(_lnaState, _workPath + "../Data");
    gbLuaCPP_dofile(_lnaState, _workPath + "../Script/LNA/gbLNA.lua");
    

    gbSvrIOEventDispatcher::Instance().Initialize(16);
    gbSvrIODataDispatcher::Instance().Initialize(16);
//    gbUDPDataHandler::Instance().Initialize(16);
    
    for(int i = 0; i < gb_SVR_LOGIC_UDP_HANDLER_ACTOR_NUM; i++)
    {
	_lnaSubState[i] = lua_newthread(_lnaState);
    }

    return true;
}
