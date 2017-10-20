#pragma once

typedef unsigned int gbCommunicatorAddr;
typedef unsigned char gbMessageType;

class gbCommunicatorMsg
{
    
};
class gbCommunicator
{
public:
    //unconnected method, maybe connected method, like connect to dest first, and then just send?
    void SendTo(const communicatorAddr dstAddr);
    virtual void Recv(void* argObj);
    
    void SubscribeMessage(const communicatorAddr dstAddr, const messageType msgType);//can be implemented based on sendto

private:
    gbCommunicatorAddr _addr;//initialize with 0
private:

    void _encode();
    gbCommunicatorAddr _decode();
};

//sample
struct gbMyRole:gbCommunicator
{
    char* name;
    int hp;
    int mp;

    void move(unsigned int delta)
	{
	    SendTo(...);
	}

    virtual void Recv(void* argObj) override
	{
	    gbCommunicatorAddr otherSide = Recv();


	    SendTo(otherSide, ...);
	}
};
