#include "gbTCPPkgHandler.h"
#include "gbSvrNet.h"
#include "gbSvrLogic.h"
gbTCPPkgActorMsg::gbTCPPkgActorMsg(gbTCPPkgHandlerData* data):
    _data(data),
    _processed(false)
{}

gbTCPPkgActorMsg::gbTCPPkgActorMsg(const gbTCPPkgActorMsg & other):
    _data(other._data),
    _processed(other._processed)
{}



gbTCPPkgActorMsg::~gbTCPPkgActorMsg()
{
    
}

void gbTCPPkgActorMsg::Process(const unsigned int actorIdx)
{
    //lock for processing datas from same socket serially
    //possible issue: if there are too many datas from same socket are loaded to different actors,
    //then it may cause too many actors to be busy at the same time, and corrupt concurrency finally.
    //solution: optimize dispatching method(the same socket data will be dispatched to the same actor when there is a same socket already occupying an actor),
    //using gbSerializeactordispatcher, then all datas with same socket will be handled sequencially, and there is no more need lock operation
    
//    std::lock_guard<std::mutex> lck(_data->socketData->GetMtx());

    if(_data->GetType() == gbTCPPkgHandlerDataType::READ)
    {
	_readProcess(actorIdx);
    }
    else
	_writeProcess(actorIdx);

}

void gbTCPPkgActorMsg::_readProcess(const unsigned int actorIdx)
{
    gbTCPSocketData* socketData = _data->GetTCPSocketData();
    std::vector<unsigned char>& remainder = socketData->GetRemainderData();
    unsigned char* buffer = socketData->GetRecvBuffer();
    unsigned int lenRcv = 0;
    gb_socket_t socket = socketData->GetSocket();
    gb_array<unsigned char>& curAppPkgData = socketData->GetCurAppPkgData();
    
    //read all avaliable data
    for(;;)
    {
	lenRcv = recv(socket, buffer, gb_TCPPKG_MAX_SIZE, 0);
	if(lenRcv == -1)
	{
	    gbLog::Instance().Error("recv error");
	    return;
	}
	//todo, optimaize data copy
	remainder.insert(remainder.end(), buffer, buffer + lenRcv);
	if(lenRcv < gb_TCPPKG_MAX_SIZE)
	    break;
    }

    unsigned char* data = remainder.data();
    unsigned int lenData = remainder.size();

    for(;;)
    {
	tcpPkgLen len = *(tcpPkgLen*)data;
	static char sizeofTcpPkgLen = sizeof(tcpPkgLen);
	if(lenData >= (len + sizeofTcpPkgLen))
	{
	    curAppPkgData.data = data;
	    curAppPkgData.length = len;
	    socketData->SetCurActorIdx(actorIdx);
	    gbAppPkg::Handle(socketData);
	    data = data + len;
	    lenData = lenData - len;
	}
	else
	    break;
    }

    remainder.erase(remainder.begin(), remainder.end() - lenData);
    
}
void gbTCPPkgActorMsg::_writeProcess(const unsigned int actorIdx)
{
    gbTCPSocketData* socketData = _data->GetTCPSocketData();
    std::queue<gb_array<unsigned char>>& qSendBuffer = socketData->GetSendBuffer();
    if(_data->GetType() == gbTCPPkgHandlerDataType::WRITABLE)
	socketData->SetWritable(true);
    else// new writedata
	qSendBuffer.push(_data->GetSendData());

    bool writable = socketData->GetWritable();

    gb_socket_t socket = socketData->GetSocket();
    gb_array<unsigned char> sd;
    unsigned int lenData;
    unsigned char* data;
    unsigned int sentLen;
    //write as much as possible
    while(writable && !qSendBuffer.empty())
    {
	sd = qSendBuffer.front();
	lenData = sd.length;
	data = sd.data;

	//send until lenData == 0, else error happenned
	while(lenData != 0)
	{
	    sentLen = send(socket, data, lenData, 0);
	    if(sentLen == -1)
		break;
	    lenData = lenData - sentLen;
	    data = data + sentLen;
	}

	if(lenData == 0)
	{
	    gbSAFE_DELETE_ARRAY(data);
	    qSendBuffer.pop();
	}
	else
	{
	    gbLog::Instance().Log("send err");
	    writable = false;
	    socketData->SetWritable(false);
	}
    }
}


gbTCPPkgHandler::~gbTCPPkgHandler()
{
    gbSAFE_DELETE(_handler);
    gbSAFE_DELETE(_dispatcher);
}

void gbTCPPkgHandler::Initialize(unsigned int num)
{
    _handler = new Theron::Framework;

    _dispatcher = new gbSerializeActorDispatcher<gbTCPPkgActorMsg*, gb_socket_t>(*_handler, num);
    _dispatcherAddr = _dispatcher->GetAddress();

    _rcvAddr = _rcv.GetAddress();
}


void gbTCPPkgHandler::Handle(const gbTCPPkgHandlerDataType type, gb_socket_t socket, gb_array<unsigned char>* sendData)
{
    TCPSocketDataItr itr = _mpTCPSocketDatas.find(socket);
    gbTCPSocketData* socketData;
    if(itr != _mpTCPSocketDatas.end())
    {
	socketData = itr->second;
    }
    else
    {
	socketData = new gbTCPSocketData(socket);
	_mpTCPSocketDatas.insert(std::pair<gb_socket_t, gbTCPSocketData*>(socket, socketData));
    }
    gbTCPPkgHandlerData* hd = new gbTCPPkgHandlerData(type, socketData, sendData);
    gbTCPPkgActorMsg* msg = new gbTCPPkgActorMsg(hd);
    _handler->Send(msg, _rcvAddr, _dispatcherAddr);
}
