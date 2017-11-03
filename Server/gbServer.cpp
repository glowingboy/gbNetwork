#include <iostream>
//#include "Core/gbSvrCore.h"
#include "../gbIOEvent.h"
#include "gbSvrLogic.h"

int main(int argc, char** argv)
{
    if(!gbSvrLogic::Instance().Start())
    {
	gbLog::Instance().Error("gbSvrLogic start err");
	return -1;
    }

    if(!gbIOEvent::Instance().Start("172.16.3.156",6668))
    {
	gbLog::Instance().Error("gbSvrNet start err");
	return -1;
    }

    ::getchar();
    return 0;
}
