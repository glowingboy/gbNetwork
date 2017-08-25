#include "gbRawData.h"


void gbRawDataMgr::Push(void* data, const size_t len)
{
	{
		std::lock_guard<std::mutex> lck(_mtx);
		_qRD.push(new gbRawData(data, len));
	}

	_cv.notify_one();
}

//gbRawData* gbRawDataMgr::Pop()
//{
//	std::lock_guard<std::mutex> lck(_mtx);
//	if (!_qRD.empty())
//	{
//		gbRawData* d = _qRD.front();
//		_qRD.pop();
//		return d;
//	}
//	else
//		return nullptr;
//
//}

void gbRawDataMgr::_rawData2AppPkg()
{
	gbRawDataMgr& mgr = gbRawDataMgr::Instance();
	while (true)
	{
		std::unique_lock<std::mutex> lck(mgr._mtx);
		gbRawData* rd(nullptr);
		if (!mgr._qRD.empty())
		{
			gbRawData* rd = mgr._qRD.front();
			mgr._qRD.pop();
			lck.unlock();

			
		}
		else
			mgr._cv.wait(lck);

	}
}