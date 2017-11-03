#pragma once
#include "gbCommunicator.h"
#include <string>



class gbDefaultComm: public gbCommunicator
{
public:
    inline gbDefaultComm(gbIOTunnel* ioTunnel):
	gbCommunicator(ioTunnel)
	{}
    inline virtual void Recv(const gbCommunicatorAddr fromAddr, gb_array<unsigned char>* rawDataArray) override
	{
	    gbCommMsgBegin(gbCommMsgString, msg, fromAddr, rawDataArray);
	    std::string strMsg;
	    msg.SerializeToString(&strMsg);
	    
// 	    gbLog::Instance().Error(gbString("********unkonw addr********\n
// addr: " + fromAddr + "\n
// msg byte size: ") + msg.ByteSize() + "\n
// msg as string: \n
// " + strMsg.c_str());

	    //send to opposite default addr
	    msg.set_val("error addr");
	    SendTo(_addr, msg);
	    
	    gbCommMsgEnd(rawDataArray);
	}
    
    gb_COMM_GETNAME_DEF(gbDefaultComm);

};
