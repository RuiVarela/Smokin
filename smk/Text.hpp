#pragma once

#include <string>
#include <vector>

#include <cstdint>

namespace smk
{
	using str8 = std::string;
	using str16 = std::u16string; // only used to convert to str32 
	using str32 = std::u32string;
	
	//
	// numeric conversions
	//
	str8 to_str8(int8_t val);
	str8 to_str8(int16_t val);
	str8 to_str8(int32_t val);
	str8 to_str8(int64_t val);
	str8 to_str8(uint8_t val);
	str8 to_str8(uint16_t val);
	str8 to_str8(uint32_t val);
	str8 to_str8(uint64_t val);
	str8 to_str8(size_t val);
	str8 to_str8(float val);
	str8 to_str8(double val);
	str8 to_str8(bool val);

	str32 to_str32(int8_t val);
	str32 to_str32(int16_t val);
	str32 to_str32(int32_t val);
	str32 to_str32(int64_t val);
	str32 to_str32(uint8_t val);
	str32 to_str32(uint16_t val);
	str32 to_str32(uint32_t val);
	str32 to_str32(uint64_t val);
	str32 to_str32(size_t val);
	str32 to_str32(float val);
	str32 to_str32(double val);
	str32 to_str32(bool val);

	int32_t str8toi(str8 const& val);
	int64_t str8toi64(str8 const& val);
	uint32_t str8tou(str8 const& val);
	uint64_t str8tou64(str8 const& val);
	float str8tof(str8 const& val);
	double str8tod(str8 const& val);
	bool str8tob(str8 const& val);

	int32_t str32toi(str32 const& val);
	int64_t str32toi64(str32 const& val);
	uint32_t str32tou(str32 const& val);
	uint64_t str32tou64(str32 const& val);
	float str32tof(str32 const& val);
	double str32tod(str32 const& val);
	bool str32tob(str32 const& val);

	//
	// Trim
	//
	enum class TrimType {
		TrimRight,
		TrimLeft,
		TrimBoth
	};

	void trimRight(str8& string, str8 const& spaces = "");
	void trimLeft(str8& string, str8 const& spaces = "");
	void trim(str8& string, str8 const& spaces = "");
	str8 trim(TrimType const& trim_type, str8 const& string, str8 const& spaces = "");

	void trimRight(str32& string, str32 const& spaces = U"");
	void trimLeft(str32& string, str32 const& spaces = U"");
	void trim(str32& string, str32 const& spaces = U"");
	str32 trim(TrimType const& trim_type, str32 const& string, str32 const& spaces = U"");

	//
	// Split
	//
	std::vector<str8> split(str8 const& input, str8 const& delimiter);
	std::vector<str32> split(str32 const& input, str32 const& delimiter);

	str8 extractToken(str8 const& input, str8 const& delimiter, int token_number);
	str32 extractToken(str32 const& input, str32 const& delimiter, int token_number);

	//
	// Replace
	//
	void replace(str8& target, str8 const& match, str8 const& replace_string);
	str8 replaceString(str8 target, str8 const& match, str8 const& replace_string);

	void replace(str32& target, str32 const& match, str32 const& replace_string);
	str32 replaceString(str32 target, str32 const& match, str32 const& replace_string);

	//
	// Contains
	//
	bool startsWith(str8 const& string, str8 const& prefix);
	bool endsWith(str8 const& string, str8 const& sufix);
	bool containsText(str8 const& string, str8 const& text);

	bool startsWith(str32 const& string, str32 const& prefix);
	bool endsWith(str32 const& string, str32 const& sufix);
	bool containsText(str32 const& string, str32 const& text);

	//
	// Number Checks
	//
	bool isNumber(str8 const& string);
	bool isInteger(str8 const& string);
	bool isReal(str8 const& string);
	bool isBoolean(str8 const& string);

	bool isNumber(str32 const& string);
	bool isInteger(str32 const& string);
	bool isReal(str32 const& string);
	bool isBoolean(str32 const& string);

	//
	// Case Conversions
	//
	enum class CaseType {
		Uppercase,
		Lowercase,
		CamelCase
	};

	void uppercase(str8& string);
	void lowercase(str8& string);
	void camelcase(str8& string, bool remove_spaces = false);
	str8 convertCase(CaseType const& case_type, str8 const& string);

	void uppercase(str32& string);
	void lowercase(str32& string);
	void camelcase(str32& string, bool remove_spaces = false);
	str32 convertCase(CaseType const& case_type, str32 const& string);

	//
	// Equals
	//
	bool equals(str8 const& lhs, str8 const& rhs, bool case_sensitive = true);
	bool equals(str32 const& lhs, str32 const& rhs, bool case_sensitive = true);


	//
	// join elements
	//
	template<typename T, typename C>
	std::basic_string<C> joinImplementation(std::vector<T> const& data, std::basic_string<C> const& separator) {
		std::basic_string<C> stream;

		for (size_t i = 0; i != data.size(); ++i) {
			if (i > 0) stream += separator;
			if constexpr (std::is_same<C, str8::value_type>::value) 
				stream += to_str8(data[i]);
			else if constexpr (std::is_same<C, str32::value_type>::value)
				stream += to_str32(data[i]);
			else 
				// can't use 'false' on clang -- expression has to depend on a template parameter
    			static_assert(!sizeof(T*), "Can only be used with valit text types"); 
		}

		return stream;
	}
	template<typename T> 
	str8 join(std::vector<T> const& data, str8 const& separator) { return joinImplementation(data, separator); }
	template<typename T>
	str32 join(std::vector<T> const& data, str32 const& separator) { return joinImplementation(data, separator); }


	// debugs a string, replacing non-printable characters with their hex value
	str8 debugStr(str8 const& input);
	str8 debugStr(str16 const& input);
	str8 debugStr(str32 const& input);

	//
	// Unicode conversions
	//
	str32 decodeUtf8(uint8_t const* input, size_t size, str32 const& replacement = U"?");
	str8 encodeUtf8(str32 const& input);
	str32 decodeUtf16(uint16_t const* input, size_t size);
	str16 encodeUtf16(str32 const& input);

	str8 u8To8(std::u8string const& input);
	
	str32 str8To32(str8 const& input);
	str8 str32To8(str32 const& input);
	str32 str16To32(str16 const& input);
	str16 str32To16(str32 const& input);


	//
	// glob
	//
	bool glob(str8 const& pattern, str8 const& text);
	bool glob(str32 const& pattern, str32 const& text);

	std::vector<str8> glob(str8 const& pattern, std::vector<str8> const& text);
	std::vector<str32> glob(str32 const& pattern, std::vector<str32> const& text);


    //
    // Levenshtein distance
    //
    size_t levenshteinDistance(str8 const& source, str8 const& target);
    size_t levenshteinDistance(str32 const& source, str32 const& target);
    
    std::vector<str8> similiarSort(str8 const& source, std::vector<str8> const& text);
    std::vector<str32> similiarSort(str32 const& source, std::vector<str32> const& text);
}
