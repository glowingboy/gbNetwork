#pragma once

#include "gbCommon.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "../gbType.h"
class gbIOTunnel;
class gbIOEventHandler;
class gbClntIOEventHandler
{
    friend class gbIOEventHandler;
//    SingletonDeclare_ExcludeDfnCnstrctor(gbClntIOEventHandler);
    inline gbClntIOEventHandler():
	_rw_thread(_read_write_thread),
	_bStopThread(false)
	{}
    ~gbClntIOEventHandler();
public:
    void Handle(const unsigned char type, gbIOTunnel* ioTunnel, gb_array<unsigned char>* sendData);

private:
    std::queue<gbIOTunnel*> _qReadableIOTunnels;
    struct SendData
    {
	inline SendData():
	    ioTunnel(nullptr)
	    {}
	inline SendData(gbIOTunnel* _ioTunnel, gb_array<unsigned char>* _sendData):
	    ioTunnel(_ioTunnel),
	    sendData(_sendData)
	    {}
	inline SendData(const SendData& other):
	    ioTunnel(other.ioTunnel),
	    sendData(other.sendData)
	    {}
	inline void operator=(const SendData& other)
	    {
		ioTunnel = other.ioTunnel;
		sendData = other.sendData;
	    }
	gbIOTunnel* ioTunnel;
	gb_array<unsigned char>* sendData;
    };
    
    std::queue<SendData> _qWritableIOTunnels;
    
    std::mutex _qRWIOTMtx;
    std::condition_variable _qRWIOTCV;

    std::thread _rw_thread;
    bool _bStopThread;
private:
    static void _read_write_thread();


    
};
