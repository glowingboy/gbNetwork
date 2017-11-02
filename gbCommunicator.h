#pragma once
#include "gbType.h"




typedef unsigned int gbCommunicatorAddr;
typedef unsigned char gbMessageType;

/**
   gbCommunicator msg pkg memory layout

   |1st HEADER{1st: dst addr|2nd: src addr}|2nd: MSG|
*/

static int gbCOMM_MSG_PKG_HEADERSIZE = 2 * sizeof(gbCommunicatorAddr);
#define gb_DEFAULTCOMM_ADDR 1
#include <google/protobuf/message_lite.h>
typedef ::google::protobuf::MessageLite gbCommunicatorMsg;

class gbSocket;

#include "CommMsg/gbCommMsg.pb.h"
#include "String/gbString.h"
#define gbCommMsgBegin(MsgType, var, rawDataArray, fromAddr)		\
    {									\
	MsgType var;							\
	if(!var.ParseFromArray(rawDataArray->data, rawDataArray->length)) \
	{								\
	    gbCommMsgString msg;					\
	    msg.set_val(gbString("wrong type of message from: ") + fromAddr + ", to: " + _addr); \
	    SendTo(gb_DEFAULTCOMM_ADDR, msg);				\
	    gbSAFE_DELETE(rawDataArray)					\
		return;							\
	}

#define gbCommMsgEnd(rawDataArray)		\
	gbSAFE_DELETE(rawDataArray)		\
	    }

#define gb_COMM_GETNAME_DEF(type)			\
    public:						\
    inline virtual std::string GetName() const override { return #type ; }

//each communicator can only recv one type of message,
//but can send any type of message to any communicator
    class gbCommunicator
    {
    public:
	gbCommunicator(gbSocket* socketData);
	inline virtual ~gbCommunicator(){}
	//unconnected method, maybe connected method, like connect to dest first, and then just send?
	void SendTo(const gbCommunicatorAddr dstAddr, const gbCommunicatorMsg& msg);
	inline gbCommunicatorAddr GetAddr() const { return _addr; }
	virtual void Recv(const gbCommunicatorAddr fromAddr, gb_array<unsigned char>* rawDataArray) = 0;
	virtual std::string GetName() const = 0;
    
	//void SubscribeMessage(const gbCommunicatorAddr dstAddr, const messageType msgType);//can be implemented based on sendto

    protected:
	gbCommunicatorAddr _addr;//initialize with 0
	gbSocket* _socketData;
    };
