#pragma once 
#include <stdlib.h>
#define gb_BOA_SWAP

#ifdef gb_BOA_SWAP
#ifdef _MSC_VER
#define gb_BOA_int16(val) _byteswap_ushort(val);
#define gb_BOA_int32(val) _byteswap_ulong(val);
#define gb_BOA_int64(val) _byteswap_uint64(val);
#elif __GNUC__
#define gb_BOA_int16(val) __builtin_bswap16(val);
#define gb_BOA_int32(val) __builtin_bswap32(val);
#define gb_BOA_int64(val) __builtin_bswap64(val);
#endif
#elif
gb_BOA_int16(val) val
gb_BOA_int16(val) val
gb_BOA_int16(val) val
#endif