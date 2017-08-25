#pragma once 
#include "gbCommon.h"
#include <mutex>
#include "gbRawData.h"
#include <vector>
/**
	appPkg:4 bytes length + body
*/

typedef unsigned int appPkgLen;
#define LEN_SIZE 4
class gbAppPkg
{

};

class gbAppPkgMgr
{
	SingletonDeclare(gbAppPkgMgr)

public:
	void Decode(gbRawData* rd);
private:
	std::vector<char> _vRemainderPkg;
};