#pragma once
#include "gbCommon.h"
#include <string>
#include <unordered_map>
#include <mutex>
#include "LuaCPP/gbLuaCPP.h"
class gbAccInfo
{
public:
    gbAccInfo(const gbAccInfo & other):
	szName(std::move(other.szName)),
	szPassword(std::move(other.szPassword))
	{}
    void operator=(const gbAccInfo & other)
	{
	    szPassword = std::move(other.szPassword);
	}
    const std::string& GetName() const {return szName;}
    const std::string& GetPassword() const { return szPassword; }
private:
    const std::string szName;
    std::string szPassword;
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
    gbLuaCPP_PrvCns<gbAccInfoAccesser>
};
