#pragma once
#include "gbCommon.h"
#include <queue>
#include <vector>
#include "../gbAppPkg.h"
#include "Theron/Theron.h"
#include "gbActor.h"
#include "../gbTCPPkg.h"
#include "gbSvrNet.h"

class gbTCPSocketData
{
public:
    inline gbTCPSocketData(const evutil_socket_t socket):
	_socket(socket),
	_recvBuffer{'\0'},
	_writable(false)
	{}
    inline std::mutex& GetMtx(){ return _mtx; }
    inline std::vector<unsigned char>& GetRemainderData(){ return _remainderData; }
    inline evutil_socket_t GetSocket() { return _socket; }
    inline unsigned char* GetRecvBuffer(){ return _recvBuffer; }
    inline void SetWritable(bool bState) { _writable = bState; }
    inline bool GetWritable() { return _writable; }
    inline std::queue<gb_array<unsigned char>>& GetSendBuffer() { return _qSendBuffer; }
private:
    std::mutex _mtx;
    std::vector<unsigned char> _remainderData;
    const evutil_socket_t _socket;
    unsigned char _recvBuffer[gb_TCPPKG_MAX_SIZE];

    std::queue<gb_array<unsigned char>> _qSendBuffer;
    bool _writable;
};


// struct gbTCPRawData
// {
//     inline ~gbTCPRawData()
// 	{
// 	    gbSAFE_DELETE(data);
// 	}
//     unsigned char* data;
//     unsigned int length;
//     gbTCPSocketData* socketData;
// };


enum gbTCPPkgHandlerDataType
{
    READ = 1,
    WRITABLE, NEWWRITEDATA
};


class gbTCPPkgHandlerData
{
public:
    inline gbTCPPkgHandlerData(const gbTCPPkgHandlerDataType type, gbTCPSocketData* socketData, gb_array<unsigned char>* sendData):
	_type(type)
	{
	    if(sendData != nullptr)
		_sendData = *sendData;
	}
    inline gbTCPPkgHandlerDataType GetType() { return _type; }
    inline gb_array<unsigned char>& GetSendData(){ return _sendData; }
    inline gbTCPSocketData* GetTCPSocketData(){ return _socketData; }
private:
    const gbTCPPkgHandlerDataType _type;
    gb_array<unsigned char> _sendData;
    gbTCPSocketData* _socketData;
};

typedef unsigned int tcpPkgLen;


class gbTCPPkgActorMsg
{
public:
    gbTCPPkgActorMsg(gbTCPPkgHandlerData* data);
    gbTCPPkgActorMsg(const gbTCPPkgActorMsg & other);
    ~gbTCPPkgActorMsg();
    void Process(const unsigned int actorIdx);
    inline void SetProcessed(){ _processed = true; }
    inline bool IsProcessed() const {return _processed;}
    inline gb_socket_t GetKey()const { return _data->GetTCPSocketData()->GetSocket(); }
private:
    gbTCPPkgHandlerData* _data;
    bool _processed;
private:
    void _readProcess(const unsigned int actorIdx);
    void _writeProcess(const unsigned int actorIdx);
};

class gbTCPPkgHandler
{
    SingletonDeclare(gbTCPPkgHandler);
public:
    void Initialize(unsigned int num);
    void Handle(const gbTCPPkgHandlerDataType type, gb_socket_t socket, gb_array<unsigned char>* sendData = nullptr);
    ~gbTCPPkgHandler();
private:
    Theron::Framework* _handler;
    gbSerializeActorDispatcher<gbTCPPkgActorMsg*, gb_socket_t>* _dispatcher;
    Theron::Address _dispatcherAddr;
    Theron::Receiver _rcv;
    Theron::Address _rcvAddr;

    typedef std::unordered_map<gb_socket_t, gbTCPSocketData*>::iterator TCPSocketDataItr;
    std::unordered_map<gb_socket_t, gbTCPSocketData*> _mpTCPSocketDatas;
};


