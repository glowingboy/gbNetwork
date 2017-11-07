#include "gbClntIORecvDataHandler.h"
#include "../gbIOEvent.h"
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
    gbClntIORecvDataHandler& handler = gbIOEvent::Instance().GetIOEventHandler().GetClntIORecvDataHandler();
    std::queue<gbIOTunnel*>& q = handler._qRecvDataTunnels;
    std::condition_variable& cv = handler._cv;
    std::unique_lock<std::mutex> lck(handler._mtx);

    gbIOTunnel* ioTunnel = q.front();
    bool&  bStop = handler._bStopThread;
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
