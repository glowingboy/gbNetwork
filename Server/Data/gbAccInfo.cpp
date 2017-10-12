#include "gbAccInfo.h"


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
