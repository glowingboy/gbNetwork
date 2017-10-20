#pragma once
#include "gbType.h"
#include <cstdint>

// #include "gbCommon.h"
// #include <mutex>
// #include "gbUDPData.h"
// #include <vector>
// #include <queue>

/**
   appPkg(UDP)
   X:
   appPkg:4 bytes type + lua_cmd
   
   // R:
   // appPkg:4 bytes type + lua_table_param({callback_idx , callback_param})
   R: <===> X appPkg: lua_cmd = LNA.R(lua_table_param)

   T:
   appPkg:4 bytes type + 4 bytes callback_idx + 4 bytes sizeof data + data
*/

//excute cmd(lua RPC)
#define gb_APPPKG_TYPE_X 'X'

// //excute result
// #define gb_APPPKG_TYPE_R 'R'
//R == X: LNA.R(lua_table_param)


//data transfer
#define gb_APPPKG_TYPE_T 'T'

// class gbAppPkg
// {
// public:
//     static void Handle(gbUDPData* ud);
//     static void Encode(const unsigned char* data, const unsigned int size, const unsigned char type);
// };

/**
   appPkg:4 bytes cpp_target key(std::uint32_t) + argument object
*/

#define gb_APPPKG_CALLBACKTARGETKEY_SIZE 4
//#define gb_APPPKG_CALLBACK_MAGICNUMBER 42
struct gbAppPkgCallback
{
public:
    //  inline gbAppPkgCallback():_magicNumber(gb_APPPKG_CALLBACK_MAGICNUMBER){}
    virtual void callback(void*);
//    inline bool Validating(){return _magicNumber == gb_APPPKG_CALLBACK_MAGICNUMBER; }
private:
//    const unsigned char _magicNumber;
};


class gbTCPSocketData;
class gbAppPkg
{
public:
    static void Handle(gbTCPSocketData* data);
};
