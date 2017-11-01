#include "gbReceptionistComm.h"

void gbReceptionistComm::Recv(const gbCommunicatorAddr fromAddr, gb_array<unsigned char>* rawDataArray)
{
    gbCommMsgBegin(gbCommMsgReceptionist, msg, rawDataArray, fromAddr);

    std::uint32_t code = msg.code();
    if(code & gb_RCPCOMM_CODE_QUERY)
    {
	gbCommMsgReceptionist newMsg;
	newMsg.set_code(gb_RCPCOMM_CODE_QUERY_RET);
	for(int i = 0; i < _whereUCanGo.size(); i++)
	{
	    newMsg.add_addrinfo();
	    gbCommMsgString_UInt32* addrInfo = newMsg.mutable_addrinfo(i);
	    const gbCommunicator* comm = _whereUCanGo[i];
	    addrInfo->set_strval(comm->GetName());
	    addrInfo->set_uint32val(comm->GetAddr());
	}
	SendTo(fromAddr, newMsg);
    }
    else if(code & gb_RCPCOMM_CODE_QUERY_RET)
    {
	int size = msg.addrinfo_size();
	for(int i = 0; i < size; i++)
	{
	    const gbCommMsgString_UInt32& addrInfo = msg.addrinfo(i);
	    _whereICanGo.insert(std::pair<std::string, gbCommunicatorAddr>(addrInfo.strval(), addrInfo.uint32val()));
	}
    }
    
    gbCommMsgEnd(rawDataArray);
}


void gbReceptionistComm::Initialize()
{
    
}
