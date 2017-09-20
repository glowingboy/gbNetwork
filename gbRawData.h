#pragma once 
#include "gbCommon.h"
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>

#define gb_UDP_MAX_PACKET_SIZE 512

class gbRawData
{
public:
	inline gbRawData(unsigned char* data, const size_t len):
		_data(data),
		_len(len)
	{}

	inline ~gbRawData()
	{
		gbSAFE_DELETE_ARRAY(_data);
	}
	inline void ReleaseBuffer() { gbSAFE_DELETE_ARRAY(_data);}
	inline unsigned char* Data() { return _data; }
	inline size_t Length() { return _len; }

private:
	unsigned char* _data;
	const size_t _len;
};

class gbRawDataMgr
{
	SingletonDeclare_ExcludeDfnCnstrctor(gbRawDataMgr);
private:
	gbRawDataMgr();
public:
	void Push(unsigned char* data, const size_t len);
	gbRawData* Pop();
private:
	std::thread* _workThread;
	std::mutex _mtx;
	//std::condition_variable _cv;
	std::queue<gbRawData*> _qRD;
private:
	//static void _rawData2AppPkg();
};
