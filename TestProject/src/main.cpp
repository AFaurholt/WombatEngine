#include <iostream>
#include <stdexcept>
#include "Wombat/Instance.h"

int main()
{
	Wombat::Instance instance = Wombat::Instance("TestProject", true);
	instance.OpenDebugWindow();

	std::cin;

	return 0;
}