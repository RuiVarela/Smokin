#include <cassert>
#include <iostream>

#include "String.h"

using namespace std;

int main(int argc, char* argv[])
{

	std::string one_as_string = "1.0";
	float one_as_float = Smokin::lexical_cast<float>(one_as_string);

	assert(one_as_float == 1.0f);
	assert(Smokin::lexical_cast<std::string>(2.0f) == "2" );
	assert(Smokin::lexical_cast<std::string>(2.55f) == "2.55" );

	assert(Smokin::lexical_cast<double>("3.55", 0.0) == 3.55 );
	assert(Smokin::lexical_cast<double>("a3.55", 0.0) == 0.0 );

	std::string message = "hello c++, and other worlds++one++two++three++";
	Smokin::StringElements elements = Smokin::split(message, " ");
	assert(elements[0] == "hello");
	assert(elements[1] == "c++,");
	assert(elements[2] == "and");
	assert(elements[3] == "other");
	assert(elements[4] == "worlds++one++two++three++");

	elements = Smokin::split(message, ",");
	assert(elements[0] == "hello c++");
	assert(elements[1] == " and other worlds++one++two++three++");

	elements = Smokin::split(message, "++");
	assert(elements[0] == "hello c");
	assert(elements[1] == ", and other worlds");
	assert(elements[2] == "one");
	assert(elements[3] == "two");
	assert(elements[4] == "three");

	message = "   hello   ";
	
	std::string processed = message;
	Smokin::trimRight(processed);
	assert(processed == "   hello");

	processed = message;
	Smokin::trimLeft(processed);
	assert(processed == "hello   ");

	processed = message;
	Smokin::trim(processed);
	assert(processed == "hello");

	message = "one two three";
	Smokin::replace(message, "two", "four" );
	assert(message == "one four three");

	Smokin::replace(message, "four", "" );
	assert(message == "one  three");

	Smokin::uppercase(message);
	assert(message == "ONE  THREE");

	Smokin::lowercase(message);
	assert(message == "one  three");

	cout << "Done." << endl;
	cin.get();

	return 0;
}

