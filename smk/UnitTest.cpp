#include "UnitTest.hpp"

#include <iostream>

namespace smk
{
	//
	// Test Result
	//
	std::string TestResult::value() const {
		std::stringstream string_stream;

		if (passed) {
			string_stream << "passed";
		} else {
			string_stream << location.filename << ':' << location.line_number << ':';

			if (!message.empty())
				string_stream << " : " << message;
		}

		return string_stream.str();
	}

	//
	// TestCase
	//
	void TestCase::tryAndChangeResult(TestResult const& test_result) {
		if (this->test_result.passed)
			this->test_result = test_result;
	}

	void TestCase::fail(TestResult result) {
		result.passed = false;
		tryAndChangeResult(result);
	}

	void TestCase::failIf(bool condition, TestResult result) {
		if(condition) {
			result.passed = false;
			tryAndChangeResult(result);
		}
	}

	void TestCase::passIf(bool condition, Location const& location) {
		if(!condition) {
			tryAndChangeResult(TestResult("Assertion failed.", location));
		}
	}

	void TestCase::createFailNotEqual(std::string const& expected, std::string const& actual, Location const& location) {
		std::stringstream string_stream;
		string_stream << "Assertion failed, expected : \"" << expected << "\" actual : \"" << actual << "\"";
		tryAndChangeResult(TestResult(string_stream.str(), location));
	}


	//
	// UnitTest
	//
	UnitTester::~UnitTester() {
		while (!test_cases.empty()) {
			delete (test_cases.back());
			test_cases.pop_back();
		}
	}

	UnitTester* UnitTester::instance() {
		static UnitTester unit_tester;
		return &unit_tester;
	}

	void UnitTester::addTestCase(TestCase* test_case) {
		test_cases.push_back(test_case);
	}

	void UnitTester::reportTest(TestCase* test_case) {
        std::stringstream string_stream;
        string_stream << "[" << (test_case->result().passed ? "passed" : "failed") << "] " << test_case->name();

		if (!test_case->result().passed)
            string_stream << std::endl << test_case->result().value();

		std::cout << string_stream.str() << std::endl;
	}

	void UnitTester::informStats()
	{
		unsigned int total_passed = 0;
		unsigned int total_failed = 0;

		for (TestCases::iterator iterator = test_cases.begin(); iterator != test_cases.end(); ++iterator) {
			if ((*iterator)->result().passed) 
				++total_passed;
			 else 
				++total_failed;
		}

        std::stringstream string_stream;
        string_stream << std::endl
				  << "Total : \t" << total_passed + total_failed << std::endl
				  << "Passed : \t" << total_passed << std::endl
                  << "Failed : \t" << total_failed;

		std::cout << string_stream.str() << std::endl;
	}


	void UnitTester::run() {
		for (TestCases::iterator iterator = test_cases.begin(); iterator != test_cases.end(); ++iterator) {
			//std::cout << "Running " << (*iterator)->name() << " ..." << std::endl;

			(*iterator)->pre();
			(*iterator)->run();
			(*iterator)->post();

			reportTest(*iterator);
		}

		informStats();
	}
}
