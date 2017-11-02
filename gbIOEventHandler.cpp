#include "gbIOEventHandler.h"
#ifdef gb_SVR
#include "Server/gbSvrIOEventDispatcher.h"
#elif
#endif
void gbIOEventHandler::Handle(unsigned char ioEventType, gbSocket* gb_socket, gb_array<unsigned char>* sendData = nullptr)
{
#ifdef gb_SVR
    gbSvrIOEventDispatcher::Instance().Dispatch(ioEventType, gb_socket, sendData);
#elif gb_CLNT
    
#endif
}
