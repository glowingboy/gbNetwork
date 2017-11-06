#pragma once
#include "gbCommunicator.h"

#define gb_WATCHDOGCOMM_CODE_SHUTDOWN 0x01
#define gb_WATCHDOGCOMM_CODE_RET_OK 0x02

class gbWatchdogComm: public gbCommunicator
{
public:
    inline gbWatchdogComm(gbIOTunnel* ioTunnel):
	gbCommunicator(ioTunnel)
	{}

    virtual void Recv(const gbCommunicatorAddr fromAddr, gb_array<unsigned char>* rawDataArray) override;
    gb_COMM_GETNAME_DEF(gbWatchdogComm);
};
