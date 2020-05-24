#include <iostream>
#include <stdexcept>
#include "Wombat/Instance.h"

int main()
{
	std::runtime_error("test error");
	std::cout << "Hello world from TestProject!" << std::endl;

	Wombat::Instance instance = Wombat::Instance("TestProject", true);
	instance.OpenDebugWindow();

	std::cin;
	return 0;
}