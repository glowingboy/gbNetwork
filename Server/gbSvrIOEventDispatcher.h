#pragma once
#include "gbCommon.h"
#include <queue>
#include <vector>
#include "Theron/Theron.h"
#include "gbActor.h"
#include <unordered_map>
#include "../gbSocket.h"

enum gbNWMessageType
{
    CONNECTED = 1,
    READABLE, WRITABLE, NEWWRITEDATA, DISCONNECTED
};

class gbSvrIOEventDispatcher
{
    SingletonDeclare(gbSvrIOEventDispatcher);
private:
    class Msg
    {
    public:
//	Msg(MsgData* data);
	Msg(const gbNWMessageType type, gbSocket* socketData, gb_array<unsigned char>* sendData);
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
	gbSocket* _socketData;

	bool _processed;
    private:
	void _connectProcess(const unsigned int actorIdx);
	void _readProcess(const unsigned int actorIdx);
	void _writeProcess(const unsigned int actorIdx);
    };

public:
    void Initialize(unsigned int num);
    void Dispatch(const gbNWMessageType type, gbSocket* socketData, gb_array<unsigned char>* sendData = nullptr);
    ~gbSvrIOEventDispatcher();
private:
    Theron::Framework* _handler;
    gbSerializeActorDispatcher<Msg*, gb_socket_t>* _dispatcher;
    Theron::Address _dispatcherAddr;
    Theron::Receiver _rcv;
    Theron::Address _rcvAddr;

    typedef std::unordered_map<gb_socket_t, gbSocket*>::iterator TCPSocketDataItr;
    std::unordered_map<gb_socket_t, gbSocket*> _mpTCPSocketDatas;
};


