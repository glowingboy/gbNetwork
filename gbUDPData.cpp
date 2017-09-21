#include "gbUDPData.h"
#include "gbAppPkg.h"
#include "ThreadPool/gbThreadPool.h"
gbUDPDataMgr::gbUDPDataMgr()
{
	//_workThread = new std::thread(_rawData2AppPkg);
	//if (_workThread == nullptr)
	//	throw std::exception("_workThread == nullptr");
}

// void gbUDPDataMgr::Push(unsigned char* data, const size_t len)
// {
// 	{
// 		std::lock_guard<std::mutex> lck(_mtx);
// 		_qUDPData.push(new gbUDPData(data, len));
// 	}

// 	gbThreadPool::Instance().PushTask(gbTask(gbAppPkgMgr::HandleRawData, nullptr));
// 	//_cv.notify_one();
// }

void gbUDPDataMgr::Push(gbUDPData* udpData)
{
    {
	std::lock_guard<std::mutex> lck(_mtx);
	_qUDPData.push(udpData);
    }
    gbThreadPool::Instance().PushTask(gbTask(gbAppPkgMgr::HandleUDPData, nullptr));
}

gbUDPData* gbUDPDataMgr::Pop()
{
	std::lock_guard<std::mutex> lck(_mtx);
	if (!_qUDPData.empty())
	{
		gbUDPData* d = _qUDPData.front();
		_qUDPData.pop();
		return d;
	}
	else
		return nullptr;
}

//void gbRawDataMgr::_rawData2AppPkg()
//{
//	gbRawDataMgr& mgr = gbRawDataMgr::Instance();
//	while (true)
//	{
//		std::unique_lock<std::mutex> lck(mgr._mtx);
//		if (!mgr._qRD.empty())
//		{
//			gbRawData* rd = mgr._qRD.front();
//			mgr._qRD.pop();
//			lck.unlock();
//			
//			gbAppPkgMgr::Instance().Decode(rd);
//			//rd->ReleaseBuffer();
//		}
//		else
//			mgr._cv.wait(lck);
//
//	}
//}
