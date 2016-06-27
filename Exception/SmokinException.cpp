#ifdef _MSC_VER

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif	

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#endif //_MSC_VER

#include <iostream>
#include "BaseExceptions.h"

void pseudoNew()
{
	throw Smokin::AllocationException(123, "Unable to allocate memory");
}

void callPseudoNew()
{
	try	
	{
		pseudoNew();
	} 
	catch (Smokin::AllocationException const& exception)
	{
		std::cout << "Smokin::AllocationException caught on callPseudoNew()" << std::endl;
		std::cout << exception << std::endl;

		throw Smokin::MemoryFullException(321, "Memory is full", exception);
	}
}

int main()
{

#ifdef _MSC_VER
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif //_MSC_VER

	Smokin::Exception exception_0("first...");
	Smokin::Exception exception_1(2, "second...");
	Smokin::Exception exception_2(3);

	std::cout << exception_0 << std::endl;
	std::cout << exception_1 << std::endl;
	std::cout << exception_2 << std::endl;

	Smokin::Exception grandparent(4, "grandparent");
	Smokin::Exception father(5, "father", grandparent);
	Smokin::Exception son(6, "son", father);
	
	std::cout << son << std::endl;

	Smokin::Exception copy(son);
	std::cout << copy << std::endl;


	Smokin::MemoryFullException mem_full(12, "This is full", son);
	Smokin::MemoryFullException mem_full_copy(mem_full);

	std::cout << mem_full << std::endl;
	std::cout << mem_full_copy << std::endl;


	try	
	{
		callPseudoNew();
	} 
	catch (Smokin::AllocationException const& exception)
	{
		std::cout << "Smokin::AllocationException caught on main" << std::endl;
		std::cout << exception << std::endl;
	}
	catch (Smokin::MemoryFullException const& exception)
	{
		std::cout << "Smokin::MemoryFullException caught on main" << std::endl;
		std::cout << exception << std::endl;
	}
	catch (...)
	{
		std::cout << "Unhandled Exception caught on main" << std::endl;
	}

	//unsigned int *s = new unsigned int[99];
	//std::cout << s[0] << std::endl;

	std::cout << "Done." << std::endl;
	std::cin.get();


	return 0;
}
