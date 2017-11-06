#pragma once
#include "gbCommon.h"
#include "gbType.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include "gbDefaultComm.h"
#include "gbWatchdogComm.h"
#include <thread>
#include <condition_variable>
#include <mutex>

#define gb_IOTUNNEL_RECV_BUFFER_SIZE 512

class gbIOTunnelMgr;
class gbIOTunnel
{
    friend class gbIOTunnelMgr;
protected:
    gbIOTunnel(const gb_socket_t socket);
    ~gbIOTunnel();
public:
    gbCommunicatorAddr RegisterCommunicator(gbCommunicator* comm);
    inline gbCommunicator* GetCommunicator(const gbCommunicatorAddr addr)
	{
	    std::unordered_map<gbCommunicatorAddr, gbCommunicator*>::iterator itr = _mpCommunicators.find(addr);
	    return itr != _mpCommunicators.end()? itr->second : _defaultComm;
	}
    void Send(gb_array<unsigned char>* msg);
    gb_socket_t GetSocket() const { return _socket; }
//    inline std::vector<unsigned char>& GetRemainderData(){ return _remainderData; }
//    inline unsigned char* GetRecvBuffer(){ return _recvBuffer; }
//    inline std::queue<gb_array<unsigned char>*>& GetSendDataQueue(){ return _qSendData; }
    // inline void SetWritable(bool bState) { _bWritable = bState; }
    // inline bool GetWritable() const { return _bWritable; }

    //can be used to check if connected in Client
    inline bool IsWritable() const { return _bWritable; }
    void Read();
    void ReadAndProcess();
    //@param sendData, if sendData is nullptr, then there must be a new WRITABLE event generated.
    void Write(gb_array<unsigned char>* sendData);

    void ProcessRecvData();

protected:
    const gb_socket_t _socket;
    std::vector<unsigned char> _remainderData;
    std::queue<std::vector<unsigned char>> _qRecvData;
    std::mutex _qRDMtx;
    unsigned char _recvBuffer[gb_IOTUNNEL_RECV_BUFFER_SIZE];

    //need a queue to store sendData for next time when this time sent failed
    std::queue<gb_array<unsigned char>*> _qSendData;
    


    std::unordered_map<gbCommunicatorAddr, gbCommunicator*> _mpCommunicators;
    
    //with default addr 1, for handling err...
    gbCommunicator* _defaultComm;
    
    gbCommunicatorAddr _curCommAddr;

    bool _bWritable;
};

class gbClientIOTunnel: public gbIOTunnel
{
    friend class gbIOTunnelMgr;
public:
    gbClientIOTunnel(const char* szIP, const short port);
private:

};

class gbIOEvent;
class gbWatchdogIOTunnel:public gbIOTunnel
{
    friend class gbIOEvent;
    friend class gbIOTunnelMgr;
private:
    gbWatchdogIOTunnel(const gb_socket_t socket);
public:
    inline gbCommunicator* GetWatchdogComm() { return _watchdogComm; }
private:
    gbCommunicator* _watchdogComm;
};

class gbIOTunnelMgr
{
    SingletonDeclare_ExcludeDfnCnstrctor(gbIOTunnelMgr);
private:
    inline gbIOTunnelMgr():
	_watchdogSvrIOTunnel(nullptr)
	{}
public:
    //@param socket, must have been connected, if no corresponding tunnel exists, then create one
    gbIOTunnel* GetIOTunnel(const gb_socket_t connectedSocket);
    inline void AddIOTunnel(gbIOTunnel* ioTunnel)
	{
	    gb_socket_t socket = ioTunnel->GetSocket();
	    std::unordered_map<gb_socket_t, gbIOTunnel*>::iterator itr = _mpIOTunnels.find(socket);
	    if(itr == _mpIOTunnels.end())
		_mpIOTunnels.insert(std::pair<gb_socket_t, gbIOTunnel*>(socket, ioTunnel));
	}
//    gbIOTunnel* GetIOTunnel(const char* szIP, const short port);
    inline void SetWatchdogClntIOTunnel(gbWatchdogIOTunnel* ioTunnel)
	{
	    _watchdogClntIOTunnel = ioTunnel;
	    AddIOTunnel(_watchdogClntIOTunnel);
	}
    inline gbWatchdogIOTunnel* GetWatchdogClntIOTunnel()
	{
	    return _watchdogClntIOTunnel;
	}

    inline gbWatchdogIOTunnel* GetWatchdogSvrIOTunnel(const gb_socket_t connectedSocket)
	{
	    if(_watchdogSvrIOTunnel != nullptr)
		return _watchdogSvrIOTunnel;
	    else
	    {
		_watchdogSvrIOTunnel = new gbWatchdogIOTunnel(connectedSocket);
		return _watchdogSvrIOTunnel;
	    }
	}
private:
    std::unordered_map<gb_socket_t, gbIOTunnel*> _mpIOTunnels;
    gbWatchdogIOTunnel* _watchdogClntIOTunnel;

    //not added to _mpIOtunnels
    gbWatchdogIOTunnel* _watchdogSvrIOTunnel;
};


//watchdog tunnel, watchdog comm
