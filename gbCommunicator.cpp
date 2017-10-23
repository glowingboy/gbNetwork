#include "gbCommunicator.h"
#include "gbSocketData.h"
#include "Log/gbLog.h"

gb_array<unsigned char> gbCommunicatorMsg::Serialize()const
{
    return gb_array<unsigned char>();
}

gbCommunicator::gbCommunicator(gbSocketData* socketData):
    _socketData(socketData)
{
    //register addr to socketData, and return a addr
    _addr = _socketData->RegisterCommunicator(this);
}

void gbCommunicator::SendTo(const gbCommunicatorAddr dstAddr, const gbCommunicatorMsg & msg)
{
    gb_array<unsigned char> sendData(msg.Serialize());
    _socketData->Send(sendData);
}

void gbCommunicator::Recv(gb_array<unsigned char> data)
{
    gbLog::Instance().Log("comm:recv");
}
