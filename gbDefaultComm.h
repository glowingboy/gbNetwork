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
	    gbCommMsgString msg;
	    std::string strMsg;
	    if( msg.ParseFromArray(rawDataArray->data, rawDataArray->length))
	    {
		msg.SerializeToString(&strMsg);
	    }
	    else
	    {
		strMsg = (char*)rawDataArray->data;
	    }
	    gbLog::Instance().Error(gbString("********unkonw communicator addr********\n") +
				    "addr: " + fromAddr + "\n"
				    "rawDataArray length: " +  rawDataArray->length+ "\n"
				    "rawDataArray as string: \n"
				    "{"+ strMsg.c_str() + "}\n"
				    "****************");
	    
	    gbSAFE_DELETE(rawDataArray);
#ifdef gb_SVR
	    //send to opposite default addr
	    msg.set_val("error addr");
	    SendTo(_addr, msg);
#endif
	}
    
    gb_COMM_GETNAME_DEF(gbDefaultComm);

};
