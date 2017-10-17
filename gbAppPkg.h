#pragma once
#include "gbType.h"

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

struct lua_State;

class gbAppPkg
{
public:
    static void Handle(unsigned char* data, const unsigned int size, const gb_socket_t socket, lua_State* l);
};
