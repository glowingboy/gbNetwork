#include "gbClntIOEventHandler.h"
// #include "../gbIOEventHandler.h"
// #include "../gbIOTunnel.h"
#include "../gbIOEvent.h"
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
    gbClntIOEventHandler& handler = gbIOEvent::Instance().GetIOEventHandler().GetClntIOEventHandler();
    std::queue<gbIOTunnel*>& qR = handler._qReadableIOTunnels;
    std::queue<SendData>& qW = handler._qWritableIOTunnels;

    std::condition_variable& cv = handler._qRWIOTCV;
    std::unique_lock<std::mutex> lck(handler._qRWIOTMtx);
    bool bReadable = false;
    bool bWritable = false;
    gbIOTunnel* rTunnel;
    SendData wTunnel;

    bool& bStop = handler._bStopThread;
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
