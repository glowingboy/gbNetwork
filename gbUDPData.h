#pragma once 
#define gb_UDP_MAX_PACKET_SIZE 512

class gbUDPData
{
public:
    inline gbUDPData(unsigned char* data, const size_t len, const sockaddr* sockAddr, const socklen_t sockAddrLen):
	_data(data),
	_len(len),
	_sockAddr(sockAddr),
	_sockAddrLen(sockAddrLen)
	{}

    inline ~gbUDPData()
	{
	    gbSAFE_DELETE_ARRAY(_data);
	    gbSAFE_DELETE(_sockAddr)
	}
//    inline void ReleaseBuffer() { gbSAFE_DELETE_ARRAY(_data);}
    inline unsigned char* Data() { return _data; }
    inline size_t Length() { return _len; }

private:
    unsigned char* _data;
    const size_t _len;
    const sockaddr* _sockAddr;
    const socklen_t _sockAddrLen;
    unsigned int _idx;
};

// class gbUDPDataMgr
// {
//     SingletonDeclare_ExcludeDfnCnstrctor(gbUDPDataMgr);
// private:
//     gbUDPDataMgr();
// public:

//     void Push(gbUDPData* udpData);
// //    void Push(unsigned char* data, const size_t len);
//     gbUDPData* Pop();
// private:
//     std::thread* _workThread;
//     std::mutex _mtx;
//     //std::condition_variable _cv;
//     std::queue<gbUDPData*> _qUDPData;
// private:
//     //static void _rawData2AppPkg();
// };
