#include "gbAccInfo.h"
#include "Log/gbLog.h"
#include "String/gbString.h"
void gbAccInfo::gb_LC_Reg(lua_State *L)
{
    gb_LC_Func_Def;
    gb_LC_Func_push("GetPassword", &gbAccInfo::gb_LC_EF_GetPassword);
    
    gbLuaCPP<gbAccInfo>::Register(L, "gbAccInfo", funcs);
}

bool gbAccInfoAccesser::NewAccInfo(const gbAccInfo& accInfo)
{
    {
	std::lock_guard<std::mutex> lck(_accInfosMtx);	    
	accInfoIterator i = _mpAccInfos.find(accInfo.GetName());
	if(i == _mpAccInfos.end())
	{
	    _mpAccInfos.insert(std::pair<std::string, gbAccInfo>(accInfo.GetName(), accInfo));
	    return true;
	}
	else
	    return false;
    }
}
bool gbAccInfoAccesser::SetAccInfo(const gbAccInfo& accInfo)
{
    accInfoIterator i;
    {
	std::lock_guard<std::mutex> lck(_accInfosMtx);
	i = _mpAccInfos.find(accInfo.GetName());
    }
    if(i != _mpAccInfos.end())
    {
	i->second = accInfo;
	return true;
    }
    else
	return false;
}
const gbAccInfo* gbAccInfoAccesser::GetAccInfo(const char* szName)
{
    accInfoIterator i;
    {
	std::lock_guard<std::mutex> lck(_accInfosMtx);
	i = _mpAccInfos.find(szName);
    }
    if(i != _mpAccInfos.end())
	return &(i->second);
    else
	return nullptr; 
}

void gbAccInfoAccesser::gb_LC_Reg(lua_State *L)
{
    gb_LC_Func_Def;
    gb_LC_Func_push("GetAccInfo", &gbAccInfoAccesser::gb_LC_EF_GetAccInfo);
    gb_LC_Func_push("NewAccInfo", &gbAccInfoAccesser::gb_LC_EF_NewAccInfo);
    gb_LC_Func_push("SetAccInfo", &gbAccInfoAccesser::gb_LC_EF_SetAccInfo);
    
    gbLuaCPP<gbAccInfoAccesser>::Register(L, "gbAccInfoAccesser", funcs);

    gbLuaCPP<gbAccInfoAccesser>::RegisterSingleton(L, "gbAccInfoAccesser");
}
