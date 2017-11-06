#include "gbIOTunnel.h"
#include "gbIOEventHandler.h"
#ifdef gb_SVR
#include "Server/gbSvrIORecvDataDispatcher.h"
#elif gb_CLNT
#include "Client/gbClntIORecvDataHandler.h"
#endif
#include <sys/socket.h>
#include "gbWatchdogComm.h"

#include <thread>

gbIOTunnel::gbIOTunnel(const gb_socket_t socket):
    _socket(socket),
    _recvBuffer{'\0'},
    _curCommAddr(0),
    _bWritable(false)

{
    _defaultComm = new gbDefaultComm(this);
    if(_defaultComm == nullptr)
	throw std::runtime_error("_defaultComm created error");
}

gbIOTunnel::~gbIOTunnel()
{
    // for(std::unordered_map<gbCommunicatorAddr, gbCommunicator*>::iterator i = _mpCommunicators.begin(); i != _mpCommunicators.end(); i++)
    // {
    //gbSAFE_DELETE(i->second);
    // }
    //managed by creattor

    gbSAFE_DELETE(_defaultComm);
}

void gbIOTunnel::Send(gb_array<unsigned char>* msg)
{
    gbIOEventHandler::Instance().Handle(gb_IOEVENT_WRITABLE, this, msg);
}

gbCommunicatorAddr gbIOTunnel::RegisterCommunicator(gbCommunicator* comm)
{
    _curCommAddr++;
    _mpCommunicators.insert(std::pair<gbCommunicatorAddr, gbCommunicator*>(_curCommAddr, comm));
    return _curCommAddr;
}

void gbIOTunnel::Read()
{
    //read all avaliable data
    unsigned int lenRecv = 0;
    std::vector<unsigned char> recvData;
    for(;;)
    {
	lenRecv = recv(_socket, _recvBuffer, gb_IOTUNNEL_RECV_BUFFER_SIZE, 0);
	if( lenRecv == -1)
	{
	    gbLog::Instance().Error("recv error");
	    return ;
	}
	recvData.insert(recvData.end(), _recvBuffer, _recvBuffer + lenRecv);

	if( lenRecv < gb_IOTUNNEL_RECV_BUFFER_SIZE)
	{
	    break;
	}

    }

    if(recvData.size() > 0)
    {
	{
	    std::lock_guard<std::mutex> lck(_qRDMtx);
	    _qRecvData.push(std::move(recvData));
	}
    }
}

void gbIOTunnel::ReadAndProcess()
{
    Read();
#ifdef gb_SVR
    gbSvrIORecvDataDispatcher::Instance().Dispatch(this);
#elif gb_CLNT
    gbClntIORecvDataHandler::Instance().Handle(this);
#endif
    
}
void gbIOTunnel::Write(gb_array<unsigned char>* sendData)
{
    if(sendData != nullptr)
    {
	_qSendData.push(sendData);
    }
    else
    {
	_bWritable = true;
    }

    gb_array<unsigned char>* sd;
    unsigned int lenData;
    unsigned char* data;
    unsigned int sentLen;

    while(_bWritable && !_qSendData.empty())
    {
	sd = _qSendData.front();
	lenData = sd->length;
	data = sd->data;

	//send until lenData == 0, else error happenned
	while(lenData != 0)
	{
	    sentLen = send(_socket, data, lenData, 0);
	    if(sentLen == -1)
		break;
	    lenData = lenData - sentLen;
	    data = data + sentLen;
	}

	if(lenData == 0)
	{
	    gbSAFE_DELETE(sd);
	    _qSendData.pop();
	}
	else
	{
	    gbLog::Instance().Log("send err");
	    _bWritable = false;
	}
    }

}


void gbIOTunnel::ProcessRecvData()
{
    static unsigned char test = 0;//TODO, remove
   test ++;
    bool bQRDEmpty = false;
    
    {
	std::lock_guard<std::mutex> lck(_qRDMtx);
	bQRDEmpty = _qRecvData.empty();
    }
    std::vector<unsigned char> curData;
    while( ! bQRDEmpty )
    {
	{
	    std::lock_guard<std::mutex> lck(_qRDMtx);
	    curData = std::move(_qRecvData.front());
	    _qRecvData.pop();
	    bQRDEmpty = _qRecvData.empty();
	}

	_remainderData.insert(_remainderData.end(), curData.begin(), curData.end());
    }

    unsigned char* data = _remainderData.data();
    unsigned int lenData = _remainderData.size();
    
    if(lenData > 0)
    {
	for(;;)
	{
	    tcpPkgLen len = *(tcpPkgLen*)data;
	    static char sizeofTcpPkgLen = sizeof(tcpPkgLen);
	    if(lenData >= (len + sizeofTcpPkgLen))
	    {
		data += sizeofTcpPkgLen;
		lenData = lenData - sizeofTcpPkgLen;
		
		if(lenData >= gbCOMM_MSG_PKG_HEADERSIZE)
		{

		    gbCommunicatorAddr* addr = (gbCommunicatorAddr*)data;
		
		    gbCommunicator* comm = this->GetCommunicator(addr[0]);
		    if(comm != nullptr)
		    {
			gb_array<unsigned char>* rawDataArray = new gb_array<unsigned char>;
			rawDataArray->CopyFrom(data + gbCOMM_MSG_PKG_HEADERSIZE, len - gbCOMM_MSG_PKG_HEADERSIZE);
			comm->Recv(addr[1], rawDataArray);
		    }
		    else//GetCommunicator always return defaultComm when no comm marching addr found
			gbLog::Instance().Error("gbCommunicator is nullptr");
		}
		else
		    gbLog::Instance().Error("len < gbCOMM_MSG_PKG_HEADERSIZE");
	    
		data = data + len;
		lenData = lenData - len;
	    }
	    else
		break;
	}

	_remainderData.erase(_remainderData.begin(), _remainderData.end() - lenData);
    }
    test--;
}

gbClientIOTunnel::gbClientIOTunnel(const char* szIP, const short port):
    gbIOTunnel(socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0))
{
    if(_socket == -1)
    {
	gbLog::Instance().Error("gbClientiotunnel _socket == -1");
	throw std::runtime_error("gbClientiotunnel _socket == -1");
    }

    gbIOTunnelMgr::Instance().AddIOTunnel(this);
    
    //connect through a delegate tunnel(watchdog clnt tunnel)
    gbWatchdogIOTunnel* ioTunnel = gbIOTunnelMgr::Instance().GetWatchdogClntIOTunnel();
    gbCommMsgString_UInt32s msg;
    msg.set_strval(szIP);
    msg.add_uint32vals(_socket);
    msg.add_uint32vals(port);
    // std::uint32_t* socket = msg.mutable_uint32s(1);
    // msg.add_uint32s();
    // std::uint32_t* port = msg.mutable_uint32s(2);
    // *socket = _socket;
    // *port = port;
    gbCommunicator* comm = ioTunnel->GetWatchdogComm();
    comm->SendTo(comm->GetAddr(), msg);
}

gbWatchdogIOTunnel::gbWatchdogIOTunnel(const gb_socket_t socket):
    gbIOTunnel(socket)
{
    _watchdogComm = new gbWatchdogComm(this);
    if(_watchdogComm == nullptr)
	throw std::runtime_error("_watchdogComm == nullptr");
}

gbIOTunnel* gbIOTunnelMgr::GetIOTunnel(const gb_socket_t connectedSocket)
{
    std::unordered_map<gb_socket_t, gbIOTunnel*>::iterator itr = _mpIOTunnels.find(connectedSocket);
    if(itr != _mpIOTunnels.end())
	return itr->second;
    else
    {
	gbIOTunnel* tunnel = new gbIOTunnel(connectedSocket);
	if(tunnel != nullptr)
	{
	    _mpIOTunnels.insert(std::pair<gb_socket_t, gbIOTunnel*>(connectedSocket, tunnel));
	}
	return tunnel;
    }
}


// gbWatchdogIOTunnel* gbIOTunnelMgr::GetWatchdogClntIOTunnel(const gb_socket_t connectedSocket)
// {
//     if(_watchdogClntIOTunnel != nullptr)
// 	return _watchdogClntIOTunnel;
//     else
//     {
// 	_watchdogClntIOTunnel = new gbWatchdogIOTunnel(connectedSocket);
// 	return _watchdogClntIOTunnel;
//     }
// }
