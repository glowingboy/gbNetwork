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
	    gbLog::Instance().Error(gbString("********unkonw communicator addr********\n") +
				    "addr: " + fromAddr + "\n"
				    "rawDataArray length: " +  rawDataArray->length+ "\n"
				    "rawDataArray as string: \n"
				    "{"+ (char*)rawDataArray->data + "}\n"
				    "****************");
	    
	    gbSAFE_DELETE(rawDataArray);
#ifdef gb_SVR
	    //send to opposite default addr
	    gbCommMsgString msg;
	    msg.set_val("error addr");
	    SendTo(_addr, msg);
#endif
	}
    
    gb_COMM_GETNAME_DEF(gbDefaultComm);

};
