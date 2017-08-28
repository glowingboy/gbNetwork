#pragma once 
#include "gbCommon.h"
#include <mutex>
#include "gbRawData.h"
#include <vector>

/**
	appPkg:4 bytes length + body(1 byte type + szData)
*/

//data sync
#define gb_APPPKG_TYPE_S 'S'
//excute cmd(lua RPC)
#define gb_APPPKG_TYPE_X 'X'

typedef unsigned int appPkgLen;
#define gb_APPPKG_LEN_SIZE 4
class gbAppPkg
{
public:
	gbAppPkg(char* data, const appPkgLen len);
private:
	const char _type;
	char* _szData;

};

class gbAppPkgMgr
{
	SingletonDeclare(gbAppPkgMgr)

public:
	void Decode(gbRawData* rd);
private:
	std::vector<unsigned char> _vRemainderPkg;
};