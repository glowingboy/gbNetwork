#pragma once 
#include "gbCommon.h"
#include <mutex>
#include "gbRawData.h"
#include <vector>
#include <queue>

/**
	appPkg:4 bytes length + body(1 byte type + szData)
*/

//data sync
#define gb_APPPKG_TYPE_S 'S'
//excute cmd(lua RPC)
#define gb_APPPKG_TYPE_X 'X'
//file transfer
#define gb_APPPKG_TYPE_T 'T'

typedef unsigned int appPkgLen;
#define gb_APPPKG_LEN_SIZE 4
class gbAppPkg
{
public:
	//gbAppPkg(unsigned char* data, const appPkgLen len);
	static void Handle(unsigned char* data, const appPkgLen len);
//private:
//	const unsigned char _type;
//	unsigned char* _szData;

};

class gbAppPkgMgr
{
	SingletonDeclare(gbAppPkgMgr)

public:
	//@param maxCount, if maxCount == -1, then drain out the rawData queue
	void HandleRawData(const unsigned int maxCount);
	void Decode(gbRawData* rd);
private:
	std::vector<unsigned char> _vRemainderPkg;
	//std::queue<gbAppPkg*> _qAppPkgs;//todo, priority queue?
};