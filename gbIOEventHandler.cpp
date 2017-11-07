#include "gbIOEventHandler.h"
#ifdef gb_SVR
#include "Server/gbSvrIOEventDispatcher.h"
#elif gb_CLNT
#include "Client/gbClntIOEventHandler.h"
#endif
void gbIOEventHandler::Handle(const unsigned char ioEventType, gbIOTunnel* ioTunnel, gb_array<unsigned char>* sendData)const
{
#ifdef gb_SVR
    gbSvrIOEventDispatcher::Instance().Dispatch(ioEventType, ioTunnel, sendData);
#elif gb_CLNT
    gbClntIOEventHandler::Instance().Handle(ioEventType, ioTunnel, sendData);
#endif
}