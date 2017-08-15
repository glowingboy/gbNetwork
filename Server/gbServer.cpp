#include <iostream>
#include "Core/gbSvrCore.h"
int main(int argc, char** argv)
{
	if (gbSvrCore::Initialize())
		gbSvrCore::Run(6666);
  return 0;
}
