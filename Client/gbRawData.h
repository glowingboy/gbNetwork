#pragma once 
#include "gbCommon.h"
#include <mutex>
#include <queue>
#include <condition_variable>

class gbRawData
{
public:
	inline gbRawData(void* data, const size_t len):
		_data(data),
		_len(len)
	{}

	inline ~gbRawData()
	{
		gbSAFE_DELETE_ARRAY(_data);
	}

private:
	void* _data;
	const size_t _len;
};

class gbRawDataMgr
{
	SingletonDeclare(gbRawDataMgr)
public:
	void Push(void* data, const size_t len);
	//gbRawData* Pop();
private:
	std::mutex _mtx;
	std::condition_variable _cv;
	std::queue<gbRawData*> _qRD;
private:
	static void _rawData2AppPkg();
};