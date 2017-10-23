#pragma once
#include "gbCommunicator.h"

struct gbDefaultCommMsg: public gbCommunicatorMsg
{
    
}

class gbDefaultComm: public gbCommunicator
{
public:
    virtual Recv(gb_array<unsigned char> data) override;

};
