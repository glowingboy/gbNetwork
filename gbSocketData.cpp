#include "gbSocketData.h"
#include "Server/gbNWMessageDispatcher.h"

gbSocketData::gbSocketData(const gb_socket_t socket):
    _socket(socket),
    _recvBuffer{'\0'},
    _writable(false),
    _curCommAddr(0)
{
    _defaultComm = new gbCommunicator(this);
    if(_defaultComm == nullptr)
	throw std::runtime_error("_defaultComm created error");
}

gbSocketData::~gbSocketData()
{
    // for(std::unordered_map<gbCommunicatorAddr, gbCommunicator*>::iterator i = _mpCommunicators.begin(); i != _mpCommunicators.end(); i++)
    // {
    //gbSAFE_DELETE(i->second);
    // }
    //managed by creattor

    gbSAFE_DELETE(_defaultComm);
}

void gbSocketData::Send(gb_array<unsigned char>& msg)
{
    gbNWMessageDispatcher::Instance().Dispatch(gbNWMessageType::NEWWRITEDATA, this, &msg);   
}

gbCommunicatorAddr gbSocketData::RegisterCommunicator(gbCommunicator* comm)
{
    _curCommAddr++;
    _mpCommunicators.insert(std::pair<gbCommunicatorAddr, gbCommunicator*>(_curCommAddr, comm));
    return _curCommAddr;
}

gbSocketData* gbSocketDataMgr::GetSocketData(const gb_socket_t socket)
{
    std::unordered_map<gb_socket_t, gbSocketData*>::iterator itr = _mpSocketDatas.find(socket);
    if(itr != _mpSocketDatas.end())
	return itr->second;
    else
    {
	gbSocketData* sd = new gbSocketData(socket);
	if(sd != nullptr)
	{
	    _mpSocketDatas.insert(std::pair<gb_socket_t, gbSocketData*>(socket, sd));
	}
	return sd;
    }
}
