#pragma once
#include "gbCommon.h"
#include "LuaCPP/gbLuaCPP.h"
#include <String/gbString.h>
class gbSvrLogic
{
    SingletonDeclare(gbSvrLogic);
public:
    bool Start();
private:
    gbString _workPath;
};
