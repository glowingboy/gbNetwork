#include "gbNWMessageDispatcher.h"
#include "gbSvrNet.h"
#include "gbSvrLogic.h"

#include "gbReceptionistComm.h"

gbSvrIOEventDispatcher::Msg::Msg(const gbNWMessageType type, gbSocket* socketData, gb_array<unsigned char>* sendData):
    _type(type),
    _sendData(sendData),
    _socketData(socketData),
    _processed(false)
{
}

gbSvrIOEventDispatcher::Msg::Msg(const Msg & other):
    _type(other._type),
    _sendData(other._sendData),
    _socketData(other._socketData),
    _processed(other._processed)
{
    
}



// Msg::~Msg()
// {
    
// }

void gbSvrIOEventDispatcher::Msg::Process(const unsigned int actorIdx)
{
    //lock for processing datas from same socket serially
    //possible issue: if there are too many datas from same socket are loaded to different actors,
    //then it may cause too many actors to be busy at the same time, and corrupt concurrency finally.
    //solution: optimize dispatching method(the same socket data will be dispatched to the same actor when there is a same socket already occupying an actor),
    //using gbSerializeactordispatcher, then all datas with same socket will be handled sequencially, and there is no more need lock operation
    
//    std::lock_guard<std::mutex> lck(_data->socketData->GetMtx());
    if(_type == gbNWMessageType::CONNECTED)
    {
	_connectProcess(actorIdx);
    }
    else if(_type == gbNWMessageType::READABLE)
    {
	_readProcess(actorIdx);
    }
    else
	_writeProcess(actorIdx);

}

void gbSvrIOEventDispatcher::Msg::_connectProcess(const unsigned int actorIdx)
{
    gbReceptionistComm* rcpComm = new gbReceptionistComm(_socketData);
    rcpComm->Initialize();
}

void gbSvrIOEventDispatcher::Msg::_readProcess(const unsigned int actorIdx)
{
// //    gbSocketData* socketData = _data->GetTCPSocketData();
//     std::vector<unsigned char>& remainder = _socketData->GetRemainderData();
//     unsigned char* buffer = _socketData->GetRecvBuffer();
//     unsigned int lenRcv = 0;
//     gb_socket_t socket = _socketData->GetSocket();
// //    gb_array<unsigned char>& curAppPkgData = socketData->GetCurAppPkgData();
    
//     //read all avaliable data
//     for(;;)
//     {
// 	lenRcv = recv(socket, buffer, gb_TCPPKG_MAX_SIZE, 0);
// 	if(lenRcv == -1)
// 	{
// 	    gbLog::Instance().Error("recv error");
// 	    return;
// 	}
// 	//todo, optimaize data copy
// 	remainder.insert(remainder.end(), buffer, buffer + lenRcv);
// 	if(lenRcv < gb_TCPPKG_MAX_SIZE)
// 	    break;
//     }

//     unsigned char* data = remainder.data();
//     unsigned int lenData = remainder.size();

//     for(;;)
//     {
// 	tcpPkgLen len = *(tcpPkgLen*)data;
// 	static char sizeofTcpPkgLen = sizeof(tcpPkgLen);
// 	if(lenData >= (len + sizeofTcpPkgLen))
// 	{
// 	    // curAppPkgData.data = data;
// 	    // curAppPkgData.length = len;
// 	    // socketData->SetCurActorIdx(actorIdx);
// 	    // gbAppPkg::Handle(socketData);
// 	    if(len >= gbCOMM_MSG_PKG_HEADERSIZE)
// 	    {
// 		gbCommunicatorAddr* addr = (gbCommunicatorAddr*)data;
		
// 		gbCommunicator* comm = _socketData->GetCommunicator(addr[0]);
// 		if(comm != nullptr)
// 		{
// 		    gb_array<unsigned char>* rawDataArray = new gb_array<unsigned char>;
// 		    rawDataArray->CopyFrom(data + gbCOMM_MSG_PKG_HEADERSIZE, len - gbCOMM_MSG_PKG_HEADERSIZE);
// 		    comm->Recv(addr[1], rawDataArray);
		    
// 		}
// 		else
// 		    gbLog::Instance().Error("gbCommunicator is nullptr");
// 	    }
// 	    else
// 		gbLog::Instance().Error("len < gbCOMM_MSG_PKG_HEADERSIZE");
	    
// 	    data = data + len;
// 	    lenData = lenData - len;
// 	}
// 	else
// 	    break;
//     }

//     remainder.erase(remainder.begin(), remainder.end() - lenData);
    
}
void gbSvrIOEventDispatcher::Msg::_writeProcess(const unsigned int actorIdx)
{
// //    gbSocketData* socketData = _data->GetTCPSocketData();
//     std::queue<gb_array<unsigned char>*>& qSendBuffer = _socketData->GetSendDataQueue();
//     if(_type == gbNWMessageType::WRITABLE)
// 	_socketData->SetWritable(true);
//     else// new writedata
// 	qSendBuffer.push(_sendData);

//     bool writable = _socketData->GetWritable();

//     gb_socket_t socket = _socketData->GetSocket();
//     gb_array<unsigned char>* sd;
//     unsigned int lenData;
//     unsigned char* data;
//     unsigned int sentLen;
//     //write as much as possible
//     while(writable && !qSendBuffer.empty())
//     {
// 	sd = qSendBuffer.front();
// 	lenData = sd->length;
// 	data = sd->data;

// 	//send until lenData == 0, else error happenned
// 	while(lenData != 0)
// 	{
// 	    sentLen = send(socket, data, lenData, 0);
// 	    if(sentLen == -1)
// 		break;
// 	    lenData = lenData - sentLen;
// 	    data = data + sentLen;
// 	}

// 	if(lenData == 0)
// 	{
// 	    gbSAFE_DELETE_ARRAY(sd);
// 	    qSendBuffer.pop();
// 	}
// 	else
// 	{
// 	    gbLog::Instance().Log("send err");
// 	    writable = false;
// 	    _socketData->SetWritable(false);
// 	}
//     }
}


gbSvrIOEventDispatcher::~gbSvrIOEventDispatcher()
{
    gbSAFE_DELETE(_handler);
    gbSAFE_DELETE(_dispatcher);
}

void gbSvrIOEventDispatcher::Initialize(unsigned int num)
{
    _handler = new Theron::Framework;

    _dispatcher = new gbSerializeActorDispatcher<Msg*, gb_socket_t>(*_handler, num);
    _dispatcherAddr = _dispatcher->GetAddress();

    _rcvAddr = _rcv.GetAddress();
}


void gbSvrIOEventDispatcher::Dispatch(const gbNWMessageType type, gbSocket* socketData, gb_array<unsigned char>* sendData)
{
    // TCPSocketDataItr itr = _mpTCPSocketDatas.find(socket);
    // gbSocketData* socketData;
    // if(itr != _mpTCPSocketDatas.end())
    // {
    // 	socketData = itr->second;
    // }
    // else
    // {
    // 	socketData = new gbSocketData(socket);
    // 	_mpTCPSocketDatas.insert(std::pair<gb_socket_t, gbSocketData*>(socket, socketData));
    // }
    // MsgData* hd = new MsgData(type, socketData, sendData);
    // Msg* msg = new Msg(hd);
    _handler->Send(Msg(type, socketData, sendData), _rcvAddr, _dispatcherAddr);
}
