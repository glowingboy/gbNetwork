#include "gbNWClient.h"
#include <iostream>
#include "../gbAppPkg.h"
#include <thread>
#include <chrono>
int main(int argc, char** argv)
{
	// if (!gbClientCore::Initialize())
	// 	return -1;
	// gbClientCore c;
	// c.Connect("172.16.3.156", "6668");

	// char ind;
	// std::cin>>ind;
	// while(true)
	// {
	//     std::this_thread::sleep_for(std::chrono::milliseconds(10));
	//     unsigned char* rd;
	//     size_t rdSize;
	//     gbAppPkgMgr::Instance().Encode("gbAccMgr:Signup(\"gb\", 123)", 'X', rd, rdSize);
	//     gbSendPkg* pkg = new gbSendPkg(rd, rdSize, false, gbSendPkg::Priority::mid);
	
	//     c.Send(pkg);
	// }
	
	// ::getchar();

    gbNWClient c;
    if(!c.Initialize())
	return -1;
    c.Connect("172.16.3.156", 6668);
    
    char r;
    std::cin >> r;

    c.Close();
    std::cout << "close end" << std::endl;
    
    return 0;
}
