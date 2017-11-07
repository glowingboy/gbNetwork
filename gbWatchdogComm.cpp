#include "gbWatchdogComm.h"
#include "gbIOEvent.h"
void gbWatchdogComm::Recv(const gbCommunicatorAddr fromAddr, gb_array<unsigned char>* rawDataArray)
{
    gbCommMsgBegin(gbCommMsgString_UInt32s, msg, fromAddr, rawDataArray);
    
    if(msg.uint32vals_size() > 1)// it's a connection cmd
    {
	gb_socket_t socket = msg.uint32vals(0);
	unsigned short port = msg.uint32vals(1);
	const char* ip = msg.strval().data();
	gbIOEvent::Instance().Connect(socket, ip, port);
    }
    else
    {
	if(msg.uint32vals_size() > 0)
	{
	    std::uint32_t code = msg.uint32vals(0);
	    if(code & gb_WATCHDOGCOMM_CODE_SHUTDOWN)
	    {
		gbIOEvent::Instance().Shutdown();
	    }
	}
	else
	{
	    gbLog::Instance().Log(msg.strval().data());
	}
    }

    gbCommMsgEnd(rawDataArray);
}
