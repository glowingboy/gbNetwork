#pragma once
/**
   For different user, frameDatas within a interval(like 100ms) are treated as simultaneously occurred. And, each user can only have one package data in one frameData. So, in one frameData, different users' data are processed concurrently, but outside, frameDatas are sequentialy processed ordered by time.

   If no frameData concept, there will be no simultaneousness, and no concurrency therefore. Because the package datas may have some logic in time, and can't preserve this logic when processed concurrently. 
*/

#ifdef __GNUC__
#include <stdint.h>
#endif
#include <map>
#include "gbCommon.h"

#define gb_FRAMEDATA_INTERVAL 100

class gbFrameData
{
public:
    void PushData();
    void Process();
};

class gbFrameDataMgr
{
    SingletonDeclare(gbFrameDataMgr);
public:
    gbFrameData* GetFrameData(int64_t time);
    void Process();
private:
    std::map<int64_t, gbFrameData*> _mpFrameDatas;
};
