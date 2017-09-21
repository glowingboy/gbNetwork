#pragma once 
#include "gbCommon.h"
#include <mutex>
#include "gbUDPData.h"
#include <vector>
#include <queue>

/**
   appPkg(TCP):4 bytes length + body(1 byte type + szData)
   appPkg(UDP):body(1 byte type + szData)
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
//    static void Handle(unsigned char* data, const appPkgLen len);
    static void Handle(gbUDPData* ud);
//private:
//	const unsigned char _type;
//	unsigned char* _szData;

};



class gbAppPkgMgr
{
    SingletonDeclare(gbAppPkgMgr)

    public:
    //@param maxCount, if maxCount == -1, then drain out the rawData queue
    static void HandleUDPData(void* p);
    void _handleUDPData(const unsigned int maxCount);
    //for tcp
//    void Decode(gbUDPData* rd);
    void Decode_udp(gbUDPData* ud);
    void Encode(const char* szData, const unsigned char type, unsigned char*& rawData, size_t & rdSize);
private:
    //for tcp
//    std::vector<unsigned char> _vRemainderPkg;
    //std::queue<gbAppPkg*> _qAppPkgs;//todo, priority queue?
};
