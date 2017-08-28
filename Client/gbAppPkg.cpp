#include "gbAppPkg.h"
#include "gbRawData.h"
#include "../gbByteOrderAmend.h"
#include <algorithm>

gbAppPkg::gbAppPkg(char* data, const appPkgLen len):
	_type(*data)
{
	_szData = new char[len];
	memcpy(_szData, data + 1, len - 1);
	_szData[len - 1] = '\0';
}
void gbAppPkgMgr::Decode(gbRawData* rd)
{
	if (_vRemainderPkg.size() == 0)
	{
		unsigned char* data = (unsigned char*)(rd->Data());
		const size_t rdLen = rd->Length();
		appPkgLen len = gb_BOA_int32(*(appPkgLen*)data);
		if (len + gb_APPPKG_LEN_SIZE > rdLen)
			_vRemainderPkg.insert(_vRemainderPkg.end(), data, data + rdLen);
		else
		{

		}
	}
}