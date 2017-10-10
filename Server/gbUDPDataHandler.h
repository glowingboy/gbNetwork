#pragma once
#include "../gbUDPData.h"
#include "gbCommon.h"
#include "Theron/Theron.h"

class gbUDPDataHandler
{
    SingletonDeclare(gbUDPDataHandler);
public:
    void Handle(gbUDPData* udpData);
};


class gbUDHDispatcher: public Theron::Actor
{
public:
    gbUDHDispatcher(Theron::Framework & framework):
	Theron::Actor(framework)
	{
	    
	}
};
