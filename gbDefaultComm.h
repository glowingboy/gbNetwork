#pragma once
#include "gbCommunicator.h"
#include "Log/gbLog.h"
#include "String/gbString.h"
#include <string>

#include "CommMsg/gbDefaultCommMsg.pb.h"

class gbDefaultComm: public gbCommunicator
{
public:
    inline gbDefaultComm(gbSocketData* socketData):
	gbCommunicator(socketData)
	{}
    inline virtual void Recv(const gbCommunicatorAddr fromAddr, gb_array<unsigned char>* rawDataArray) override
	{
	    gbCommMsgBegin(gbDefaultCommMsg, msg, rawDataArray);
	    std::string strMsg;
	    msg.SerializeToString(&strMsg);
	    
// 	    gbLog::Instance().Error(gbString("********unkonw addr********\n
// addr: " + fromAddr + "\n
// msg byte size: ") + msg.ByteSize() + "\n
// msg as string: \n
// " + strMsg.c_str());

	    SendTo(fromAddr, msg);
	    
	    gbCommMsgEnd(rawDataArray);
	}

};
