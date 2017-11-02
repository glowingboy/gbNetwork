#pragma once

#include "gbCommon.h"
#include "gbType.h"

#define gb_IOEVENT_READABLE 0x2
#define gb_IOEVENT_WRITABLE 0x4


class gbIOEventHandler
{
    SingletonDeclare(gbIOEventHandler);
public:
    void Handle(unsigned char ioEventType, gbSocket* gb_socket, gb_array<unsigned char>* sendData = nullptr);
};
