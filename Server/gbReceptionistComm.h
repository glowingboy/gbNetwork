#pragma once

/**
   both of tow side communicators have a receptionist which is the first communicator 
   where both can talk with each other at the connected point.
*/
#include "../gbCommunicator.h"
class gbReceptionistComm: public gbCommunicator
{
public:
    gbReceptionistComm(gbSocketData* socketData):
	gbCommunicator(socketData)
	{}
    
    
};
