#pragma once
#include "gbCommon.h"
#include <queue>
#include <vector>
#include "gbActor.h"
#include <unordered_map>
#include "../gbIOTunnel.h"
#include <mutex>
class gbSvrIOEventDispatcher
{
    SingletonDeclare(gbSvrIOEventDispatcher);
private:
    class Msg
    {
    public:
//	Msg(MsgData* data);
	Msg(const unsigned char type, gbIOTunnel* ioTunnel, gb_array<unsigned char>* sendData);
	Msg(const Msg & other);
	//~Msg();
	void Process(const unsigned int actorIdx);
	inline void SetProcessed(){ _processed = true; }
	inline bool IsProcessed() const {return _processed;}
	inline gb_socket_t GetKey()const { return _ioTunnel->GetSocket(); }
    private:
//	MsgData* _data;

	const unsigned char _type;
	gb_array<unsigned char>* _sendData;
	gbIOTunnel* _ioTunnel;

	bool _processed;
    // private:
    // 	void _connectProcess(const unsigned int actorIdx);
    // 	void _readProcess(const unsigned int actorIdx);
    // 	void _writeProcess(const unsigned int actorIdx);
    };

public:
    void Initialize(unsigned int num);
    void Dispatch(const unsigned char type, gbIOTunnel* ioTunnel, gb_array<unsigned char>* sendData = nullptr);
    ~gbSvrIOEventDispatcher();
private:
    Theron::Framework* _framework;
    gbSerializeActorDispatcher<Msg, gb_socket_t>* _dispatcher;
    Theron::Address _dispatcherAddr;
    Theron::Receiver _rcv;
    Theron::Address _rcvAddr;

    // typedef std::unordered_map<gb_socket_t, gbSocket*>::iterator TCPSocketDataItr;
    // std::unordered_map<gb_socket_t, gbSocket*> _mpTCPSocketDatas;

    std::mutex _mtx;
};


