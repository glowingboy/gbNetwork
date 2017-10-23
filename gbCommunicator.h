#pragma once
#include "gbType.h"

typedef unsigned int gbCommunicatorAddr;
typedef unsigned char gbMessageType;

struct gbCommunicatorMsg
{
    gb_array<unsigned char> Serialize()const;
};

class gbSocketData;

class gbCommunicator
{
public:
    gbCommunicator(gbSocketData* socketData);
    inline virtual ~gbCommunicator(){}
    //unconnected method, maybe connected method, like connect to dest first, and then just send?
    void SendTo(const gbCommunicatorAddr dstAddr, const gbCommunicatorMsg& msg);
    virtual void Recv(gb_array<unsigned char> data);
    
    //void SubscribeMessage(const gbCommunicatorAddr dstAddr, const messageType msgType);//can be implemented based on sendto

private:
    gbCommunicatorAddr _addr;//initialize with 0
    gbSocketData* _socketData;
private:

    void _encode();
    gbCommunicatorAddr _decode();
};

//sample
// struct gbMyRole:gbCommunicator
// {
//     char* name;
//     int hp;
//     int mp;

//     void move(unsigned int delta)
// 	{
// 	    SendTo(...);
// 	}

//     virtual void Recv(void* argObj) override
// 	{
// 	    gbCommunicatorAddr otherSide = Recv();


// 	    SendTo(otherSide, ...);
// 	}
// };
