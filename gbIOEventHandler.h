#pragma once

#include "gbCommon.h"
#include "gbType.h"

#define gb_IOEVENT_READABLE 0x2
#define gb_IOEVENT_WRITABLE 0x4

class gbIOTunnel;
class gbIOEventHandler
{
    SingletonDeclare(gbIOEventHandler);
public:
    void Handle(const unsigned char ioEventType, gbIOTunnel* ioTunnel, gb_array<unsigned char>* sendData = nullptr)const;
};
