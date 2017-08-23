#include "Core/gbClientCore.h"
#include <iostream>

int main(int argc, char** argv)
{
	if (!gbClientCore::Initialize())
		return -1;
	gbClientCore c;
	c.Connect("127.0.0.1", 6666);
	char inBuf[1024] = { '\0' };
	int i = 0;
	int id = ::GetTickCount();

	char test[1024 * 100] = { '2' };
	if (!c.Send(test, 1024 * 100))
		std::cout << "error" << std::endl;
	getchar();
	while (true)
	{
		//::Sleep(10);
		sprintf(inBuf, "id@%d, msg:%d\n", id, i++);
		//std::cin >> inBuf;
		if (!c.Send(inBuf, strlen(inBuf)))
			std::cout << "error" << std::endl;
	}
}
