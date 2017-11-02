#pragma once
#include "gbCommon.h"
#include "gbType.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include "gbDefaultComm.h"
#include <thread>
#include <condition_variable>
#include <mutex>
#define gb_SOCKETDATA_RECV_MAX_BUFFER_SIZE 512

class gbSocket
{
public:
    gbSocket(const gb_socket_t socket);
    ~gbSocket();
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
private:
    const gb_socket_t _socket;
    std::vector<unsigned char> _remainderData;
    std::queue<std::vector<unsigned char>> _qRecvData;
    std::mutex _qRDMtx;
    unsigned char _recvBuffer[gb_SOCKETDATA_RECV_MAX_BUFFER_SIZE];

    //need a queue to store sendData for next time when this time sent failed
    std::queue<gb_array<unsigned char>*> _qSendData;
    


    std::unordered_map<gbCommunicatorAddr, gbCommunicator*> _mpCommunicators;
    
    //with default addr 1, for handling err...
    gbCommunicator* _defaultComm;
    
    gbCommunicatorAddr _curCommAddr;

    bool _bWritable;

private:
    void _read();
    //@sendData, if sendData is nullptr, then there must be a new WRITABLE event generated.
    void _write(gb_array<unsigned char>* sendData);

    void _processRecvData();
};

class gbSocketMgr
{
    SingletonDeclare(gbSocketMgr);
public:
    //if no corresponding scoketData exists, then create one
    gbSocket* GetSocketData(const gb_socket_t socket);

private:
    std::unordered_map<gb_socket_t, gbSocket*> _mpSocketDatas;
};
