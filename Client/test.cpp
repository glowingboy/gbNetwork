#include "Core/gbClientCore.h"
#include <iostream>
#include "../gbAppPkg.h"
int main(int argc, char** argv)
{
	if (!gbClientCore::Initialize())
		return -1;
	gbClientCore c;
	c.Connect("127.0.0.1", 6666);

	unsigned char* rd;
	size_t rdSize;
	gbAppPkgMgr::Instance().Encode("gbAccMgr:Signup(\"gb\", 123)", 'X', rd, rdSize);
	gbSendPkg* pkg = new gbSendPkg(rd, rdSize, false, gbSendPkg::Priority::mid);
	::getchar();
	c.Send(pkg);
	
	::getchar();
}
