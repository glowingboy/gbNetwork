#pragma once
#include "gbUtils/include/gbCommon.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class gbIOTunnel;
class gbIOEventHandler;
class gbClntIORecvDataHandler
{
    friend class gbIOEventHandler;
    inline gbClntIORecvDataHandler():
	_rdp_thread(_recvDataProcesse_thread),
	_bStopThread(false)
	{}
    ~gbClntIORecvDataHandler();
public:
    void Handle(gbIOTunnel* ioTunnel);

private:
    std::queue<gbIOTunnel*> _qRecvDataTunnels;
    std::mutex _mtx;
    std::condition_variable _cv;

    std::thread _rdp_thread;
    bool _bStopThread;
private:
    static void _recvDataProcesse_thread();
};
