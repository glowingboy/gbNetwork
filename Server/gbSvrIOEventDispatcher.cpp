#include "gbSvrIOEventDispatcher.h"
#include "../gbIOEventHandler.h"

gbSvrIOEventDispatcher::Msg::Msg(const unsigned char type, gbIOTunnel* ioTunnel, gb_array<unsigned char>* sendData):
    _type(type),
    _sendData(sendData),
    _ioTunnel(ioTunnel),
    _processed(false)
{
}

gbSvrIOEventDispatcher::Msg::Msg(const Msg & other):
    _type(other._type),
    _sendData(other._sendData),
    _ioTunnel(other._ioTunnel),
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
    
//    std::lock_guard<std::mutex> lck(_data->gb_socket->GetMtx());

    if(_type & gb_IOEVENT_READABLE)
	_ioTunnel->ReadAndProcess();
    else if( _type & gb_IOEVENT_WRITABLE)
    {
	_ioTunnel->Write(_sendData);
    }
}

// void gbSvrIOEventDispatcher::Msg::_connectProcess(const unsigned int actorIdx)
// {
//     gbReceptionistComm* rcpComm = new gbReceptionistComm(_gb_socket);
//     rcpComm->Initialize();
// }

// void gbSvrIOEventDispatcher::Msg::_readProcess(const unsigned int actorIdx)
// {
// // //    gbSocketData* gb_socket = _data->GetTCPSocketData();
// //     std::vector<unsigned char>& remainder = _gb_socket->GetRemainderData();
// //     unsigned char* buffer = _gb_socket->GetRecvBuffer();
// //     unsigned int lenRcv = 0;
// //     gb_socket_t socket = _gb_socket->GetSocket();
// // //    gb_array<unsigned char>& curAppPkgData = gb_socket->GetCurAppPkgData();
    
// //     //read all avaliable data
// //     for(;;)
// //     {
// // 	lenRcv = recv(socket, buffer, gb_TCPPKG_MAX_SIZE, 0);
// // 	if(lenRcv == -1)
// // 	{
// // 	    gbLog::Instance().Error("recv error");
// // 	    return;
// // 	}
// // 	//todo, optimaize data copy
// // 	remainder.insert(remainder.end(), buffer, buffer + lenRcv);
// // 	if(lenRcv < gb_TCPPKG_MAX_SIZE)
// // 	    break;
// //     }

// //     unsigned char* data = remainder.data();
// //     unsigned int lenData = remainder.size();

// //     for(;;)
// //     {
// // 	tcpPkgLen len = *(tcpPkgLen*)data;
// // 	static char sizeofTcpPkgLen = sizeof(tcpPkgLen);
// // 	if(lenData >= (len + sizeofTcpPkgLen))
// // 	{
// // 	    // curAppPkgData.data = data;
// // 	    // curAppPkgData.length = len;
// // 	    // gb_socket->SetCurActorIdx(actorIdx);
// // 	    // gbAppPkg::Handle(gb_socket);
// // 	    if(len >= gbCOMM_MSG_PKG_HEADERSIZE)
// // 	    {
// // 		gbCommunicatorAddr* addr = (gbCommunicatorAddr*)data;
		
// // 		gbCommunicator* comm = _gb_socket->GetCommunicator(addr[0]);
// // 		if(comm != nullptr)
// // 		{
// // 		    gb_array<unsigned char>* rawDataArray = new gb_array<unsigned char>;
// // 		    rawDataArray->CopyFrom(data + gbCOMM_MSG_PKG_HEADERSIZE, len - gbCOMM_MSG_PKG_HEADERSIZE);
// // 		    comm->Recv(addr[1], rawDataArray);
		    
// // 		}
// // 		else
// // 		    gbLog::Instance().Error("gbCommunicator is nullptr");
// // 	    }
// // 	    else
// // 		gbLog::Instance().Error("len < gbCOMM_MSG_PKG_HEADERSIZE");
	    
// // 	    data = data + len;
// // 	    lenData = lenData - len;
// // 	}
// // 	else
// // 	    break;
// //     }

// //     remainder.erase(remainder.begin(), remainder.end() - lenData);
    
// }
// void gbSvrIOEventDispatcher::Msg::_writeProcess(const unsigned int actorIdx)
// {
// // //    gbSocketData* gb_socket = _data->GetTCPSocketData();
// //     std::queue<gb_array<unsigned char>*>& qSendBuffer = _gb_socket->GetSendDataQueue();
// //     if(_type == gbNWMessageType::WRITABLE)
// // 	_gb_socket->SetWritable(true);
// //     else// new writedata
// // 	qSendBuffer.push(_sendData);

// //     bool writable = _gb_socket->GetWritable();

// //     gb_socket_t socket = _gb_socket->GetSocket();
// //     gb_array<unsigned char>* sd;
// //     unsigned int lenData;
// //     unsigned char* data;
// //     unsigned int sentLen;
// //     //write as much as possible
// //     while(writable && !qSendBuffer.empty())
// //     {
// // 	sd = qSendBuffer.front();
// // 	lenData = sd->length;
// // 	data = sd->data;

// // 	//send until lenData == 0, else error happenned
// // 	while(lenData != 0)
// // 	{
// // 	    sentLen = send(socket, data, lenData, 0);
// // 	    if(sentLen == -1)
// // 		break;
// // 	    lenData = lenData - sentLen;
// // 	    data = data + sentLen;
// // 	}

// // 	if(lenData == 0)
// // 	{
// // 	    gbSAFE_DELETE_ARRAY(sd);
// // 	    qSendBuffer.pop();
// // 	}
// // 	else
// // 	{
// // 	    gbLog::Instance().Log("send err");
// // 	    writable = false;
// // 	    _gb_socket->SetWritable(false);
// // 	}
// //     }
// }


gbSvrIOEventDispatcher::~gbSvrIOEventDispatcher()
{
    gbSAFE_DELETE(_framework);
    gbSAFE_DELETE(_dispatcher);
}

void gbSvrIOEventDispatcher::Initialize(unsigned int num)
{
    _framework = new Theron::Framework;

    _dispatcher = new gbSerializeActorDispatcher<Msg, gb_socket_t>(*_framework, num);
    _dispatcherAddr = _dispatcher->GetAddress();

    _rcvAddr = _rcv.GetAddress();
}


void gbSvrIOEventDispatcher::Dispatch(const unsigned char type, gbIOTunnel* ioTunnel, gb_array<unsigned char>* sendData)
{
    // TCPSocketDataItr itr = _mpTCPSocketDatas.find(socket);
    // gbSocketData* gb_socket;
    // if(itr != _mpTCPSocketDatas.end())
    // {
    // 	gb_socket = itr->second;
    // }
    // else
    // {
    // 	gb_socket = new gbSocketData(socket);
    // 	_mpTCPSocketDatas.insert(std::pair<gb_socket_t, gbSocketData*>(socket, gb_socket));
    // }
    // MsgData* hd = new MsgData(type, gb_socket, sendData);
    // Msg* msg = new Msg(hd);
    std::lock_guard<std::mutex> lck(_mtx);
    _framework->Send(Msg(type, ioTunnel, sendData), _rcvAddr, _dispatcherAddr);
}
