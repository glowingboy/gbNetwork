#pragma once 
#include "gbCommon.h"
#include <mutex>
#include "gbUDPData.h"
#include <vector>
#include <queue>

/**
   appPkg(TCP):4 bytes length + body(1 byte type + szData)
   appPkg(UDP):4 bytes idx + body(1 byte type + szData)
*/

//excute cmd(lua RPC)
#define gb_APPPKG_TYPE_X 'X'
//data transfer
#define gb_APPPKG_TYPE_T 'T'

typedef unsigned int appPkgLen;
#define gb_APPPKG_LEN_SIZE 4
class gbAppPkg
{
public:
    static void Handle(gbUDPData* ud);
    static void Encode(const unsigned char* data, const unsigned int size, const unsigned char type);
};
