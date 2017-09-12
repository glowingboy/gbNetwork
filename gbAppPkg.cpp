#include "gbAppPkg.h"
#include "gbRawData.h"
#include "../gbByteOrderAmend.h"
#include <algorithm>
#include "LuaCPP/gbLuaCPP.h"
//gbAppPkg::gbAppPkg(unsigned char* data, const appPkgLen len):
//	_type(*data)
//{
//	_szData = new unsigned char[len];
//	memcpy(_szData, data + 1, len - 1);
//	_szData[len - 1] = '\0';
//}
void gbAppPkg::Handle(unsigned char* data, const appPkgLen len)
{
	const unsigned char type = *data;
	if (type == 'S')
	{

	}
	else if (type == 'X')
	{
		
	}
}
void gbAppPkgMgr::HandleRawData(const unsigned int maxCount)
{
	unsigned int curCount = 0;
	while (true)
	{
		gbRawData* rData = gbRawDataMgr::Instance().Pop();
		if (rData == nullptr)
			break;
		Decode(rData);
		rData->ReleaseBuffer();
		curCount++;
		if (curCount == maxCount)
			break;
	}
	
}
void gbAppPkgMgr::Decode(gbRawData* rd)
{
	unsigned char* data = (unsigned char*)(rd->Data());
	size_t rdLen = rd->Length();
	if (_vRemainderPkg.size() != 0)
	{
		_vRemainderPkg.insert(_vRemainderPkg.end(), data, data + rdLen);
		data = _vRemainderPkg.data();
		rdLen = _vRemainderPkg.size();
	}
	
	while (true)
	{
		appPkgLen len = gb_BOA_int32(*(appPkgLen*)data);
		const size_t size = len + gb_APPPKG_LEN_SIZE;
		if (size <= rdLen)
		{
			gbAppPkg::Handle(data + gb_APPPKG_LEN_SIZE, len);
			//_qAppPkgs.push(new gbAppPkg(data + gb_APPPKG_LEN_SIZE, len));
			data += size;
			rdLen = rdLen - size;
			if (rdLen == 0)
				break;
		}
		else
		{
			_vRemainderPkg.insert(_vRemainderPkg.end(), data, data + rdLen);
			break;
		}
	}
}