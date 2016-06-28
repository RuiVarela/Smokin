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

#include "XmlSerialization.h"
using namespace std;

class MyType
{
public:
	MyType()
	{
		one = 1;
		two = 2.2f;
		three = 3.3;
		four = "4";
	}

	bool isValid()
	{
		bool state = true;

		state &= (one == 1);
		state &= (two == 2.2f);
		state &= (three == 3.3);
		state &= (four == "4");

		return state;
	}

	void clear()
	{
		one = 0;
		two = 0;
		three = 0;
		four = "";
	}

	int one;
	float two;
	double three;
	std::string four; 
};

//
// Serialization method outside the class.
//
namespace Smokin
{
	namespace Serialization
	{
		void serialize(XmlArchive& archive, MyType& test)
		{
			SMOKIN_SERIALIZE(archive, test.one);
			SMOKIN_SERIALIZE(archive, test.two);
			SMOKIN_SERIALIZE(archive, test.three);
			SMOKIN_SERIALIZE(archive, test.four);
		}
	};
};


//
// Serialization method inside the class.
//

class MySecondType
{
public :
	MySecondType(bool do_clear = true)
	{
		one = 101;
		two = 102.2f;
		three = 103.3;
		four = "104";
		
		if (do_clear)
		{
			clear();
		}
	}

	bool isValid()
	{
		bool state = true;
		
		state &= (one == 101);
		state &= (two == 102.2f);
		state &= (three == 103.3);
		state &= (four == "104");

		state &= my_test.isValid();

		return state;
	}
	
	void clear()
	{
		one = 0;
		two = 0;
		three = 0;
		four = "";

		my_test.clear();
	}

	int one;
	float two;
	double three;
	std::string four;

	MyType my_test;


	//
	// Serialization method inside the class.
	//
	void serialize(Smokin::XmlArchive& archive)
	{
		SMOKIN_SERIALIZE(archive, one);
		SMOKIN_SERIALIZE(archive, two);
		SMOKIN_SERIALIZE(archive, three);
		SMOKIN_SERIALIZE(archive, four);
		SMOKIN_SERIALIZE(archive, my_test);
	}

};

int main(int argc, char* argv[])
{
#ifdef _MSC_VER
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif //_MSC_VER

	// writing
	{
		Smokin::XmlArchive archive;
		assert(archive.open("dump.xml", Smokin::XmlArchive::Write));

		int my_integer = 2;
		double my_double = 23.67;
		float my_float = 45.0089f;
		std::string my_string = "my string!";
		MyType my_test;
		MySecondType my_second_test(false);

		Smokin::serialize(archive, "my_named_integer", my_integer); //manually naming
		SMOKIN_SERIALIZE(archive, my_integer); // auto naming
		SMOKIN_SERIALIZE(archive, my_double);
		SMOKIN_SERIALIZE(archive, my_float);
		SMOKIN_SERIALIZE(archive, my_string);
		SMOKIN_SERIALIZE(archive, my_test);
		SMOKIN_SERIALIZE(archive, my_second_test);

		std::vector<int> int_vector;
		std::list<int> int_list;
		std::vector<MySecondType> my_second_type_vector;
		unsigned int const max = 10;
			
		for (unsigned int i = 0; i != max; ++i)
		{
			int_vector.push_back(max - i);
			int_list.push_back(max - i);
			my_second_type_vector.push_back(MySecondType(false));
		}

		SMOKIN_SERIALIZE(archive, int_vector);
		SMOKIN_SERIALIZE(archive, int_list);
		SMOKIN_SERIALIZE(archive, my_second_type_vector);

		assert(archive.save());
	}

	//
	// reading
	//

	{
		int my_integer = 0;
		double my_double = 0;
		float my_float = 0;
		std::string my_string;

		Smokin::XmlArchive archive;
		assert(archive.open("dump.xml"));

		Smokin::serialize(archive, "my_named_integer", my_integer); //manually naming
		assert(my_integer == 2);
		my_integer = 0;
		MyType my_test;
		MySecondType my_second_test;

		my_test.clear();
		my_second_test.clear();

		SMOKIN_SERIALIZE(archive, my_integer); // auto naming
		assert(my_integer == 2);
		SMOKIN_SERIALIZE(archive, my_double);
		assert(my_double == 23.67);
		SMOKIN_SERIALIZE(archive, my_float);
		assert(my_float == 45.0089f);
		SMOKIN_SERIALIZE(archive, my_string);
		assert(my_string == "my string!");

		SMOKIN_SERIALIZE(archive, my_test);
		SMOKIN_SERIALIZE(archive, my_second_test);

		assert(my_test.isValid());
		assert(my_second_test.isValid());

		std::vector<int> int_vector;
		std::list<int> int_list;
		std::vector<MySecondType> my_second_type_vector;
		unsigned int const max = 10;
		
		SMOKIN_SERIALIZE(archive, int_vector);
		SMOKIN_SERIALIZE(archive, int_list);
		SMOKIN_SERIALIZE(archive, my_second_type_vector);
			
		for (unsigned int i = 0; i != max; ++i)
		{
			assert(int_vector[i] == (max - i));

			assert(my_second_type_vector[i].isValid());
		}


		unsigned int counter = 0;
		for (std::list<int>::iterator iterator = int_list.begin();
			iterator != int_list.end();
			++iterator)
		{
			assert(*iterator == (max - counter));
			++counter;
		}
	}

	cout << "All done." << endl;
	cin.get();

	return 0;
}

