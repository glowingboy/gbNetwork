#include <iostream>
//#include "Core/gbSvrCore.h"
#include "../gbIOEvent.h"
#include "gbSvrLogic.h"
#include <csignal>

void signal_cb(int signum)
{
    gbLog::Instance().Error(gbString("signal: ") + signum);
    exit(signum);
}
int main(int argc, char** argv)
{
    signal(SIGINT, signal_cb);
    signal(SIGPIPE, signal_cb);
    if(!gbSvrLogic::Instance().Start())
    {
	gbLog::Instance().Error("gbSvrLogic start err");
	return -1;
    }

    if(!gbIOEvent::Instance().Start("172.16.3.180",6668))
    {
	gbLog::Instance().Error("gbSvrNet start err");
	return -1;
    }

    ::getchar();
    return 0;
}
