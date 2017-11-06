#include "gbClntIOEventHandler.h"
#include "../gbIOEventHandler.h"
#include "../gbIOTunnel.h"
void gbClntIOEventHandler::Handle(const unsigned char type, gbIOTunnel* ioTunnel, gb_array<unsigned char>* sendData)
{
    if(type & gb_IOEVENT_READABLE)
    {
	{
	    std::lock_guard<std::mutex> lck(_qRWIOTMtx);
	    _qReadableIOTunnels.push(ioTunnel);
	}

	_qRWIOTCV.notify_one();
    }
    else if(type & gb_IOEVENT_WRITABLE)
    {
	{
	    std::lock_guard<std::mutex> lck(_qRWIOTMtx);
	    _qWritableIOTunnels.push(SendData(ioTunnel, sendData));
	}

	_qRWIOTCV.notify_one();
    }
}

gbClntIOEventHandler::~gbClntIOEventHandler()
{
    {
	std::lock_guard<std::mutex> lck(_qRWIOTMtx);
	_bStopThread = true;
    }

    _qRWIOTCV.notify_one();

    _rw_thread.join();
}

void gbClntIOEventHandler::_read_write_thread()
{
    std::queue<gbIOTunnel*>& qR = gbClntIOEventHandler::Instance()._qReadableIOTunnels;
    std::queue<SendData>& qW = gbClntIOEventHandler::Instance()._qWritableIOTunnels;

    std::condition_variable& cv = gbClntIOEventHandler::Instance()._qRWIOTCV;
    std::unique_lock<std::mutex> lck(gbClntIOEventHandler::Instance()._qRWIOTMtx);
    bool bReadable = false;
    bool bWritable = false;
    gbIOTunnel* rTunnel;
    SendData wTunnel;

    bool& bStop = gbClntIOEventHandler::Instance()._bStopThread;
    for(;;)
    {
	cv.wait(lck, [&]()->bool
		{
		    if(bStop)
			return true;
		    bReadable = !qR.empty();
		    if(bReadable)
		    {
			rTunnel = qR.front();
			qR.pop();
		    }

		    bWritable = !qW.empty();
		    if(bWritable)
		    {
			wTunnel = qW.front();
			qW.pop();
		    }
		    return bReadable || bWritable;
		});

	lck.unlock();

	if(bStop)
	    break;
	
	if(bReadable)
	    rTunnel->ReadAndProcess();
	if(bWritable)
	    wTunnel.ioTunnel->Write(wTunnel.sendData);
	
	lck.lock();
    }
}
