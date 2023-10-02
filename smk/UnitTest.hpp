#pragma once

#include <vector>
#include <string>
#include <sstream>

namespace smk
{
	//
	// TestResult
	//
	struct Location
	{
		Location() :line_number(0) {}
		Location(int const line, std::string const& file) :line_number(line), filename(file){}
		~Location() {}

		int line_number;
		std::string filename;
	};

	struct TestResult
	{
		TestResult() :passed(true) {}
		TestResult(bool const test_passed) :passed(test_passed) {}
		TestResult(std::string const& test_message, Location const& test_location) :passed(false), message(test_message), location(test_location) {}
		TestResult(Location const& test_location) :passed(false), location(test_location) {}

		std::string value() const;

		bool passed;
		std::string message;
		Location location;
	};

	//
	// TestCase
	//
	class TestCase
	{
	public:
		TestCase() {}
		virtual ~TestCase() {}

		virtual std::string name() const { return "TestCase"; }

		virtual void pre() {}
		virtual void run() {}
		virtual void post() {}

		TestResult const& result() { return test_result; }
		void tryAndChangeResult(TestResult const& test_result);

		// results
		void passIf(bool condition, Location const& location);

		void fail(TestResult result);
		void failIf(bool condition, TestResult result);

		void failNotEqual(int const& expected, int const& actual, Location const& location) { return failNotEqual<int>(expected, actual, location); }
		void failNotEqual(float const& expected, float const& actual, Location const& location) { return failNotEqual<float>(expected, actual, location); }

		template <typename T>
		void failNotEqual(T const* expected, std::basic_string<T> const& actual, Location const& location) { return failNotEqual(std::basic_string<T>(expected), actual, location); }

		template <typename T>
		void failNotEqual(std::basic_string<T> const& expected, T const* actual, Location const& location) { return failNotEqual(expected, std::basic_string<T>(actual), location); }

		template <typename T>
		void failNotEqual(T expected, T actual, Location const& location) {
			if (expected != actual)
				createFailNotEqual(asString(expected), asString(actual), location);
		}
	protected:
		void createFailNotEqual(std::string const& expected, std::string const& actual, Location const& location);
		TestResult test_result;

		
		template <typename T>
		std::string asString(std::basic_string<T> const& value) {
			std::stringstream string_stream;
			for (auto const& character : value) {
				 int v = (unsigned int)(character);

				 if constexpr (std::is_same<T, std::string::value_type>::value) {
					 v &= 0x000000FF;
				 } else if constexpr (std::is_same<T, std::u32string::value_type>::value) {

				 } else {
					 // can't use 'false' on clang -- expression has to depend on a template parameter
					 static_assert(!sizeof(T*), "ut does not support this type");
				 }

				if (v < 127)
					string_stream << (unsigned char)(character);
				else
					string_stream << "[" << v << "]";
			}
			return string_stream.str();
		}
		
		template <typename T>
		std::string asString(T const& value) {
			std::stringstream string_stream;
			string_stream << value;
			return string_stream.str();
		}

	};

	//
	// UnitTester
	//

	class UnitTester
	{
	public:
		typedef std::vector<TestCase*> TestCases;

		UnitTester() { }
		~UnitTester();

		void addTestCase(TestCase* test_case);

		void reportTest(TestCase* test_case);
		void informStats();

		void run();

		static UnitTester* instance();
	private:
		TestCases test_cases;
	};

	//
	// TestCaseAutoRegister
	//

	template <typename T>
	struct TestCaseAutoRegister
	{
		TestCaseAutoRegister()
		{
			UnitTester::instance()->addTestCase(new T());
		}
		~TestCaseAutoRegister() { }
	};
}

//
// Helper Macros
//
#define TEST_BEGIN(test_class) \
	struct test_class : public smk::TestCase { \
		virtual std::string name() const { return #test_class; } \
		virtual void run() {

#define TEST_END(test_class) \
		} \
	}; \
	static smk::TestCaseAutoRegister<test_class> test##test_class;


//
// Assertion
//

#define TEST_LOCATION() smk::Location( __LINE__, __FILE__ )
#define TEST_ASSERT(condition) passIf(condition, TEST_LOCATION())
#define TEST_FAIL(message) fail(smk::TestResult(message, TEST_LOCATION()))
#define TEST_FAIL_IF(condition, message) failIf(condition, smk::TestResult(message, TEST_LOCATION()))
#define TEST_FAIL_NOT_EQUAL(expected, actual) failNotEqual(expected, actual, TEST_LOCATION())
