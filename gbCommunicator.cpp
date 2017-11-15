#include "gbCommunicator.h"
#include "gbIOTunnel.h"
#include "gbUtils/include/gbLog.h"

gbCommunicator::gbCommunicator(gbIOTunnel* ioTunnel):
    _ioTunnel(ioTunnel)
{
    //register addr to socketData, and return a addr
    _addr = _ioTunnel->RegisterCommunicator(this);
}

void gbCommunicator::SendTo(const gbCommunicatorAddr dstAddr, const gbCommunicatorMsg & msg)
{
    int byteSize = msg.ByteSize();
    static char sizeofTCPPkgLen = sizeof(tcpPkgLen);
    int dataSize = byteSize + gbCOMM_MSG_PKG_HEADERSIZE + sizeofTCPPkgLen;
    unsigned char* data = new unsigned char[dataSize];
    if(data != nullptr)
    {
	gb_array<unsigned char>* sendData = new gb_array<unsigned char>(data, dataSize);
	
	*(tcpPkgLen*)data = byteSize + gbCOMM_MSG_PKG_HEADERSIZE;
	data += sizeofTCPPkgLen;

	gbCommunicatorAddr* addrs = reinterpret_cast<gbCommunicatorAddr*>(data);
	addrs[0] = dstAddr;
	addrs[1] = _addr;
	msg.SerializeToArray(data + gbCOMM_MSG_PKG_HEADERSIZE, byteSize);
	
	_ioTunnel->Send(sendData);
    }
    else
	throw std::runtime_error("gbCommunicator::SendTo new data err");
}

