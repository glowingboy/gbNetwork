#include "gbCommunicator.h"
#include "gbIOTunnel.h"
#include "Log/gbLog.h"

gbCommunicator::gbCommunicator(gbIOTunnel* ioTunnel):
    _ioTunnel(ioTunnel)
{
    //register addr to socketData, and return a addr
    _addr = _ioTunnel->RegisterCommunicator(this);
}

void gbCommunicator::SendTo(const gbCommunicatorAddr dstAddr, const gbCommunicatorMsg & msg)
{
    gb_array<unsigned char>* sendData = new gb_array<unsigned char>;
    int size = msg.ByteSize();
    unsigned char* data = new unsigned char[size + gbCOMM_MSG_PKG_HEADERSIZE];
    if(data != nullptr)
    {
	gbCommunicatorAddr* addrs = reinterpret_cast<gbCommunicatorAddr*>(data);
	addrs[0] = dstAddr;
	addrs[1] = _addr;
	msg.SerializeToArray(data + gbCOMM_MSG_PKG_HEADERSIZE, size);
	_ioTunnel->Send(sendData);
    }
    else
	throw std::runtime_error("gbCommunicator::SendTo new data err");
}

