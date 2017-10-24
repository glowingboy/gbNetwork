#pragma once
#include "gbType.h"




typedef unsigned int gbCommunicatorAddr;
typedef unsigned char gbMessageType;

/**
   gbCommunicator msg pkg memory layout

   |1st HEADER{1st: dst addr|2nd: src addr}|2nd: MSG|
*/

static int gbCOMM_MSG_PKG_HEADERSIZE = 2 * sizeof(gbCommunicatorAddr);

#include <google/protobuf/message_lite.h>
typedef ::google::protobuf::MessageLite gbCommunicatorMsg;

class gbSocketData;

#define gbCommMsgBegin(MsgType, var, rawDataArray)			\
    {							\
    MsgType var;					\
    var.ParseFromArray(rawDataArray->data, rawDataArray->length); 

#define gbCommMsgEnd(rawDataArray)				\
    gbSAFE_DELETE(rawDataArray)				\
    }


class gbCommunicator
{
public:
    gbCommunicator(gbSocketData* socketData);
    inline virtual ~gbCommunicator(){}
    //unconnected method, maybe connected method, like connect to dest first, and then just send?
    void SendTo(const gbCommunicatorAddr dstAddr, const gbCommunicatorMsg& msg);
    virtual void Recv(const gbCommunicatorAddr fromAddr, gb_array<unsigned char>* rawDataArray) = 0;
    
    //void SubscribeMessage(const gbCommunicatorAddr dstAddr, const messageType msgType);//can be implemented based on sendto

private:
    gbCommunicatorAddr _addr;//initialize with 0
    gbSocketData* _socketData;
};
