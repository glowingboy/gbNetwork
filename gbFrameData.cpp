#include "gbFrameData.h"


gbFrameData* gbFrameDataMgr::GetFrameData(int64_t time)
{
    for(std::map<int64_t, gbFrameData*>::iterator i = _mpFrameDatas.begin(); i != _mpFrameDatas.end(); i++)
    {
	if(std::abs(time - i->first) <= gb_FRAMEDATA_INTERVAL)
	    return i->second; 
    }

    gbFrameData* fd = new gbFrameData;
    _mpFrameDatas.insert(std::pair<int64_t, gbFrameData*>(time, fd));
    return fd;
}
