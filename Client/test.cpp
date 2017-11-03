#include "../gbIOEvent.h"

#include <iostream>

#include <chrono>
int main(int argc, char** argv)
{

    gbIOEvent::Instance().Start(nullptr, 6666);
    gbIOTunnel ioTunnel("127.0.0.1", 6666);

    for(;;)
    {
	if(ioTunnel.IsWritable())
	    break;
    }

//comm = ioTunnel.AddComm(...);
    //comm send
    return 0;
}
