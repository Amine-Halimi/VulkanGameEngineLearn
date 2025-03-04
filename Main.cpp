#include "ApplicationEngine.hpp"

//std
#include "iostream"
#include "cstdlib"
#include "stdexcept"

/*
*
* Main.cpp is the entry point for the program
*
* author: Amine Halimi
*/


int main()
{
	weEngine::ApplicationEngine engine{};

	try {
		engine.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}