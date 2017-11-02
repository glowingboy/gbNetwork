#pragma once

/**
   both of tow side communicators have a receptionist which is the first communicator 
   where both can talk with each other at the connected point.
*/
#include "../gbCommunicator.h"
#include <vector>
#include <unordered_map>

/*0x00-0xff reserved*/
#define gb_RCPCOMM_CODE_QUERY 0x100
#define gb_RCPCOMM_CODE_QUERY_RET 0x200
class gbReceptionistComm: public gbCommunicator
{
public:
    gbReceptionistComm(gbSocket* socketData):
	gbCommunicator(socketData)
	{}
    
    virtual void Recv(const gbCommunicatorAddr fromAddr, gb_array<unsigned char>* rawDataArray) override;
    gbCommMsg_GetName_Def(gbReceptionistComm);
    
    void Initialize();
private:
    std::vector<gbCommunicator*> _whereUCanGo;
    std::unordered_map<std::string, gbCommunicatorAddr> _whereICanGo;
};
