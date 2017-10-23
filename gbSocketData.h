#pragma once
#include "gbCommon.h"
#include "gbType.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include "gbCommunicator.h"

#define gb_SOCKETDATA_RECV_MAX_BUFFER_SIZE 512

class gbSocketData
{
public:
    gbSocketData(const gb_socket_t socket);
    ~gbSocketData();
    gbCommunicatorAddr RegisterCommunicator(gbCommunicator* comm);
    inline gbCommunicator* GetCommunicator(const gbCommunicatorAddr addr)
	{
	    std::unordered_map<gbCommunicatorAddr, gbCommunicator*>::iterator itr = _mpCommunicators.find(addr);
	    return itr != _mpCommunicators.end()? itr->second : nullptr;
	}
    void Send(gb_array<unsigned char>& msg);
    inline gb_socket_t GetSocket()const{ return _socket; }
    inline std::vector<unsigned char>& GetRemainderData(){ return _remainderData; }
    inline unsigned char* GetRecvBuffer(){ return _recvBuffer; }
    inline std::queue<gb_array<unsigned char>>& GetSendDataQueue(){ return _qSendData; }
    inline void SetWritable(bool bState) { _writable = bState; }
    inline bool GetWritable() const { return _writable; }
private:
    const gb_socket_t _socket;
    std::vector<unsigned char> _remainderData;
    unsigned char _recvBuffer[gb_SOCKETDATA_RECV_MAX_BUFFER_SIZE];
    std::queue<gb_array<unsigned char>> _qSendData;
    bool _writable;

    std::unordered_map<gbCommunicatorAddr, gbCommunicator*> _mpCommunicators;
    
    //with default addr 1, for handling like login, query other addr...
    gbCommunicator* _defaultComm;
    
    gbCommunicatorAddr _curCommAddr;
};

class gbSocketDataMgr
{
    SingletonDeclare(gbSocketDataMgr);
public:
    //if no corresponding scoketData exists, then create one
    gbSocketData* GetSocketData(const gb_socket_t socket);

private:
    std::unordered_map<gb_socket_t, gbSocketData*> _mpSocketDatas;
};
