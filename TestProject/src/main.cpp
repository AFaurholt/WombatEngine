#include <iostream>
#include "Wombat/Instance.h"

int main()
{
	std::cout << "Hello world from TestProject!" << std::endl;
	Wombat::Instance instance = Wombat::Instance("TestProject", true);
	instance.OpenDebugWindow();
	delete &instance;
	return 0;
}