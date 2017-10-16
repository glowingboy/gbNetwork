#pragma once
#include "gbCommon.h"
#include <string>
#include <unordered_map>
#include <mutex>
#include "LuaCPP/gbLuaCPP.h"
class gbAccInfo
{
public:
    inline gbAccInfo(){}
    inline gbAccInfo(const char* name, const char* password):
	szName(name),
	szPassword(password)
	{}
    inline gbAccInfo(const gbAccInfo & other):
	szName(std::move(other.szName)),
	szPassword(std::move(other.szPassword))
	{}
    inline void operator=(const gbAccInfo & other)
	{
	    szPassword = std::move(other.szPassword);
	}
    const std::string& GetName() const {return szName;}
    const std::string& GetPassword() const { return szPassword; }

private:
    std::string szName;
    std::string szPassword;

    gb_LC_lt2st_begin;
    gb_LC_lt2st_string(szName);
    gb_LC_lt2st_string(szPassword);
    gb_LC_lt2st_end;

    gb_LC_st2lt_begin;
    gb_LC_st2lt_string(szName);
    gb_LC_st2lt_string(szPassword);
    gb_LC_st2lt_end;
    
    gb_LC_Register(gbAccInfo)

    gb_LC_EXPORT_FUNC(GetPassword)
	{
	    lua_pushstring(L, GetPassword().c_str());
	    return 1;
	}
};


class gbAccInfoAccesser
{
    SingletonDeclare(gbAccInfoAccesser);
public:
    bool NewAccInfo(const gbAccInfo& accInfo);
    bool SetAccInfo(const gbAccInfo& accInfo);
    const gbAccInfo* GetAccInfo(const char* szName);
    
private:
    typedef std::unordered_map<std::string, gbAccInfo>::iterator accInfoIterator;
    std::unordered_map<std::string, gbAccInfo> _mpAccInfos;

    std::mutex _accInfosMtx;

    //export lua api
    gb_LC_Register(gbAccInfoAccesser)

    gb_LC_EXPORT_FUNC(NewAccInfo)
	{
	    gbAccInfo info;
	    info.lt2st(L, 2);
	    lua_pushboolean(L, NewAccInfo(info));
	    return 1;
	}

    gb_LC_EXPORT_FUNC(SetAccInfo)
	{
	    gbAccInfo info;
	    info.lt2st(L, 2);
	    lua_pushboolean(L, SetAccInfo(info));
	    return 1;
	}
    
    gb_LC_EXPORT_FUNC(GetAccInfo)
	{
	    const gbAccInfo* info = GetAccInfo(lua_tostring(L, 2));
	    return info->st2lt(L) ? 1 : 0;
	}
};
