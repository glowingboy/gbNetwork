#include "gbClntIORecvDataHandler.h"
#include "../gbIOTunnel.h"
void gbClntIORecvDataHandler::Handle(gbIOTunnel* ioTunnel)
{
    if(ioTunnel != nullptr)
    {
	{
	    std::lock_guard<std::mutex> lck(_mtx);
	    _qRecvDataTunnels.push(ioTunnel);
	}

	_cv.notify_one();
    }
}


gbClntIORecvDataHandler::~gbClntIORecvDataHandler()
{
    {
	std::lock_guard<std::mutex> lck(_mtx);
	_bStopThread = true;
    }

    _cv.notify_one();
    _rdp_thread.join();
}

void gbClntIORecvDataHandler::_recvDataProcesse_thread()
{
    std::queue<gbIOTunnel*>& q = gbClntIORecvDataHandler::Instance()._qRecvDataTunnels;
    std::condition_variable& cv = gbClntIORecvDataHandler::Instance()._cv;
    std::unique_lock<std::mutex> lck(gbClntIORecvDataHandler::Instance()._mtx);

    gbIOTunnel* ioTunnel = q.front();
    bool&  bStop = gbClntIORecvDataHandler::Instance()._bStopThread;
    for(;;)
    {
	cv.wait(lck, [&]()->bool
		{
		    if(bStop)
			return true;
		    if(!q.empty())
		    {
			ioTunnel = q.front();
			q.pop();
			return true;
		    }
		    else
			return false;
		});
	lck.unlock();

	if(bStop)
	    break;
	
	ioTunnel->ProcessRecvData();

	lck.lock();
    }
}
