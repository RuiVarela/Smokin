#ifdef _MSC_VER

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif	

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#endif //_MSC_VER


#include <iostream>
#include <string>
#include <cassert>
#include "SharedPointer.h"

using namespace std;

class MyTest
{
public:
	MyTest(std::string const& name)
		:my_name(name)
	{
		std::cout << "Contructor : " << my_name << std::endl;
	}

	virtual void speak()
	{
		std::cout << "My Name is : " << my_name << std::endl;
	}

	virtual ~MyTest()
	{
		std::cout << "Destructor : " << my_name << std::endl;
	}

	std::string my_name;
};

class MyTestChild : public MyTest
{
public:
	MyTestChild(std::string const& name)
		:MyTest(name)
	{}

	virtual ~MyTestChild() {}
private:
};


void testByCopy(Smokin::SharedPointer<MyTest> by_copy, unsigned int count)
{
	assert(by_copy.referenceCount() == count);
}

void testByRef(Smokin::SharedPointer<MyTest>& ref, unsigned int count)
{
	assert(ref.referenceCount() == count);
	ref->speak();
}

void testByConstRef(Smokin::SharedPointer<MyTest> const& ref, unsigned int count)
{
	assert(ref.referenceCount() == count);
	ref->speak();
}

Smokin::SharedPointer<MyTest> testReturn()
{
	Smokin::SharedPointer<MyTest> my_test = new MyTest("MyTest");
	return my_test;
}

int main(int argc, char* argv[])
{
#ifdef _MSC_VER
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif //_MSC_VER


	{
		MyTest one("one");

		MyTest *two = new MyTest("two");
		Smokin::SharedPointer<MyTest> share_pointer_two(two);
		assert(share_pointer_two.referenceCount() == 1);

		Smokin::SharedPointer<MyTest> three( new MyTest("three") );
		Smokin::SharedPointer<MyTest> four( new MyTest("four") );
		assert(four.referenceCount() == 1);
		
		{
			Smokin::SharedPointer<MyTest> four_copy(four);
			assert(four.referenceCount() == four_copy.referenceCount());
			assert(four.referenceCount() == 2);

			assert(four.get() != three);
			assert(three != four.get());
			assert(three != four.operator ->());
			assert(three != four);

			assert(four_copy.get() == four);
			assert(four == four_copy.get());
			assert(four == four_copy.operator ->());
			assert(four == four_copy);
		}

		assert(four.referenceCount() == 1);
		
		Smokin::SharedPointer<MyTest> five( new MyTest("five") );
		assert(five.referenceCount() == 1);
		Smokin::SharedPointer<MyTest> six( new MyTest("six") );
		Smokin::SharedPointer<MyTest> seven;
		six = five;
		seven = six;

		assert(five.referenceCount() == 3);

		six = new MyTest("Plain Six.");
		assert(five.referenceCount() == 2);

		six = 0;
		assert(six.referenceCount() == 1);
	}

	// passing pointer to functions...
	{
		Smokin::SharedPointer<MyTest> param_0 = new MyTest("parameter 0");
		assert(param_0.referenceCount() == 1);
		testByCopy(param_0, 2);

		testByRef(param_0, 1);
		testByConstRef(param_0, 1);

		Smokin::SharedPointer<MyTest> returned = testReturn();
		returned->speak();
		assert(returned.referenceCount() == 1);
	}

	{	
		Smokin::SharedPointer<MyTestChild> child( new MyTestChild("child") );
		Smokin::SharedPointer<MyTest> father( new MyTest("father") );

		Smokin::SharedPointer<MyTest> conversion_test_0 = child;
		assert(child.referenceCount() == 2);
		Smokin::SharedPointer<MyTest> conversion_test_1;
		conversion_test_1 = child;
		assert(child.referenceCount() == 3);

		Smokin::SharedPointer<MyTestChild> conversion_test_2;

		conversion_test_2 = Smokin::dynamic_pointer_cast<MyTestChild>(conversion_test_1);
		assert(child.referenceCount() == 4);
		conversion_test_1 = 0;
		assert(child.referenceCount() == 3);

	}

	// returning pointer from function

	cout << "All done." << endl;
	cin.get();

	
	return 0;
}

