#pragma once 
#include "gbUtils/include/gbCommon.h"
#include <unordered_map>
class gbSvrUsr
{

};

class gbSvrUsrMgr
{
	SingletonDeclare(gbSvrUsrMgr)

private:
	std::unordered_map<unsigned int, gbSvrUsr*> _loggedUsrs;
};
