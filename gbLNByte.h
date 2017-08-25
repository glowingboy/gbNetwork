#pragma once 
/**
	convert local world/number to network bytes order, vice versa.

	use network byte order( big endian order)
*/

//ref https://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c
namespace gbLNByte
{
#ifdef GB_LITTLE_ENDIAN
#ifdef _MSC_VER
#define gb_ln_ushort(value) _byteswap_ushort(value);
#define gb_ln_ulong(value) _byteswap_ulong(value);
#define gb_ln_uint64(value) _byteswap_uint64(value);
#elif __GUNC__
	inline unsigned short _gb_ln_ushort_gnuc_imp(unsigned short)
	{

	}
#define gb_ln_ushort(value)
#define gb_ln_ulong(value) __builtin_bswap32(value);
#define gb_ln_uint64(value)	__builtin_bswap64(value);
#endif
#else

#endif
}