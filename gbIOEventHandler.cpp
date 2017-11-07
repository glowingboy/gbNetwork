#include "gbIOEventHandler.h"

void gbIOEventHandler::Handle(const unsigned char ioEventType, gbIOTunnel* ioTunnel, gb_array<unsigned char>* sendData)
{
#ifdef gb_SVR
    _svrDispatcher.Dispatch(ioEventType, ioTunnel, sendData);
#elif gb_CLNT
    _clntHandler.Handle(ioEventType, ioTunnel, sendData);
#endif
}
