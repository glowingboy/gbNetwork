#include "../gbIOEvent.h"

#include <iostream>
#include "../gbIOTunnel.h"
#include <chrono>
int main(int argc, char** argv)
{

    gbIOEvent::Instance().Start(nullptr, 6667);
    gbClientIOTunnel ioTunnel("172.16.3.180", 6668);

    for(;;)
    {
	if(ioTunnel.IsWritable())
	    break;
    }

    gbCommunicator* c = ioTunnel.GetCommunicator(1);
    for(;;)
    {
	gbCommMsgString msg;
	msg.set_val("hello");
	
	std::this_thread::sleep_for(std::chrono::microseconds(100));
	c->SendTo(1, msg);
    }
    
//comm = ioTunnel.AddComm(...);
    //comm send
    return 0;
}
