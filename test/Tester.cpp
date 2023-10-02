#include <iostream>
#include <UnitTest.hpp>
#include <File.hpp>

using namespace smk;

Path testRoot() {
	Path test_root = convertFileNameToNativeStyle(U"../test/");
	if (fileType(test_root) != FileType::FileDirectory)
		test_root = convertFileNameToNativeStyle(U"../../test/");
	return test_root;
}


int main()
{
	std::cout << "UnitTest Running." << std::endl;
	UnitTester::instance()->run();
	std::cout << "UnitTest Finished." << std::endl;
	return 0;
}
