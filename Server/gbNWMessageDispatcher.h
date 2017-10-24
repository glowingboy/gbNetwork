#pragma once
#include "gbCommon.h"
#include <queue>
#include <vector>
#include "../gbAppPkg.h"
#include "Theron/Theron.h"
#include "gbActor.h"
#include "../gbTCPPkg.h"
#include "gbSvrNet.h"
#include <unordered_map>
#include "../gbSocketData.h"

// class gbSocketData
// {
// public:
//     inline gbSocketData(const evutil_socket_t socket):
// 	_socket(socket),
// 	_recvBuffer{'\0'},
// 	_writable(false)
// 	{}
//     inline std::mutex& GetMtx(){ return _mtx; }
//     inline std::vector<unsigned char>& GetRemainderData(){ return _remainderData; }
//     inline evutil_socket_t GetSocket() { return _socket; }
//     inline unsigned char* GetRecvBuffer(){ return _recvBuffer; }
//     inline void SetWritable(bool bState) { _writable = bState; }
//     inline bool GetWritable() { return _writable; }
//     inline std::queue<gb_array<unsigned char>>& GetSendBuffer() { return _qSendBuffer; }
//     inline gbAppPkgCallback* GetCallbackTarget(const unsigned int key)
// 	{
// 	    std::unordered_map<unsigned int, gbAppPkgCallback*>::iterator itr = _callbackRegister.find(key);
// 	    if(itr != _callbackRegister.end())
// 		return itr->second;
// 	    else
// 		return nullptr;
// 	}
//     inline gb_array<unsigned char>& GetCurAppPkgData() { return _curAppPkgData; }
//     inline unsigned int GetCurActorIdx() { return _curActorIdx; }
//     inline void SetCurActorIdx(const unsigned int idx) { _curActorIdx = idx; }
// private:
//     std::mutex _mtx;
//     std::vector<unsigned char> _remainderData;
//     const evutil_socket_t _socket;
//     unsigned char _recvBuffer[gb_TCPPKG_MAX_SIZE];

//     std::queue<gb_array<unsigned char>> _qSendBuffer;
//     bool _writable;

//     std::unordered_map<unsigned int, gbAppPkgCallback*> _callbackRegister;

//     gb_array<unsigned char> _curAppPkgData;
    
//     unsigned int _curActorIdx;
// };


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


enum gbNWMessageType
{
    CONNECTED = 1,
    READABLE, WRITABLE, NEWWRITEDATA, DISCONNECTED
};

class gbNWMessageDispatcher
{
    SingletonDeclare(gbNWMessageDispatcher);
private:
    // class MsgData
    // {
    // public:
    // 	inline MsgData(const gbNWMessageType type, gbSocketData* socketData, gb_array<unsigned char>* sendData):
    // 	    _type(type)
    // 	    {
    // 		if(sendData != nullptr)
    // 		    _sendData = *sendData;
    // 	    }
    // 	inline gbNWMessageType GetType() { return _type; }
    // 	inline gb_array<unsigned char>& GetSendData(){ return _sendData; }
    // 	inline gbSocketData* GetTCPSocketData(){ return _socketData; }
    // private:
    // 	const gbNWMessageType _type;
    // 	gb_array<unsigned char> _sendData;
    // 	gbSocketData* _socketData;
    // };

    class Msg
    {
    public:
//	Msg(MsgData* data);
	Msg(const gbNWMessageType type, gbSocketData* socketData, gb_array<unsigned char>* sendData);
	Msg(const Msg & other);
	//~Msg();
	void Process(const unsigned int actorIdx);
	inline void SetProcessed(){ _processed = true; }
	inline bool IsProcessed() const {return _processed;}
	inline gb_socket_t GetKey()const { return _socketData->GetSocket(); }
    private:
//	MsgData* _data;

	const gbNWMessageType _type;
	gb_array<unsigned char>* _sendData;
	gbSocketData* _socketData;

	bool _processed;
    private:
	void _connectProcess(const unsigned int actorIdx);
	void _readProcess(const unsigned int actorIdx);
	void _writeProcess(const unsigned int actorIdx);
    };

public:
    void Initialize(unsigned int num);
    void Dispatch(const gbNWMessageType type, gbSocketData* socketData, gb_array<unsigned char>* sendData = nullptr);
    ~gbNWMessageDispatcher();
private:
    Theron::Framework* _handler;
    gbSerializeActorDispatcher<Msg*, gb_socket_t>* _dispatcher;
    Theron::Address _dispatcherAddr;
    Theron::Receiver _rcv;
    Theron::Address _rcvAddr;

    typedef std::unordered_map<gb_socket_t, gbSocketData*>::iterator TCPSocketDataItr;
    std::unordered_map<gb_socket_t, gbSocketData*> _mpTCPSocketDatas;
};


