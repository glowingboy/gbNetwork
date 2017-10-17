#include "gbTCPPkg.h"

// void gbTCPPkgContainer::Destroy()
// {
//     _ref_count--;
//     if(_ref_count == 0)
// 	delete this;
// }

// void gbTCPPkgContainer::PushBack(const unsigned char* data, const unsigned int len)
// {
//     _data.insert(_data.end(), data, data + len);
// }

// void gbTCPPkgMgr::Handle(const gbTCPRawData* rdata)
// {
//     std::queue<gbAppPkg*> completePkg;
//     unsigned int lenData = rdata->length;
//     unsigned char* data = rdata->data;
//     rmndrItr itr = _mpRemainder.find(rdata->socket);
//     if(itr != _mpRemainder.end())
//     {
// 	std::vector<unsigned char>& r = itr->second;
// 	r.insert(r.end(), data, data + lenData);
// 	lenData = r.size();
// 	data = r.data();
//     }

//     for(;;)
//     {
// 	tcpPkgLen len = *(tcpPkgLen*)data;
// 	static char sizeofTcpPkgLen = sizeof(tcpPkgLen);
// 	if(lenData >= (len + sizeofTcpPkgLen))
// 	{
// 	    gbAppPkg* appPkg = new gbAppPkg;
// 	    appPkg
// 	}
//     }

//     return completePkg;
// }
// gbTCPPkg gbTCPPkgMgr::Encode(const gbAppPkg* appPkg)
// {
    
// }

