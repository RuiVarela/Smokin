#include "Text.hpp"

#include <algorithm>
#include <sstream>
#include <iomanip>

namespace smk {

	//
	// Traits
	//
	template<typename T>
	struct str_traits { };

	template<>
	struct str_traits<str8::value_type> {
		constexpr static str8 defaultTrim() { return " \t\f\v\n\r"; }
		constexpr static str8 empty() { return ""; }
		constexpr static str8 space() { return " "; }
		constexpr static str8 bTrue() { return "true"; }
		constexpr static str8 bFalse() { return "false"; }
	};

	template<>
	struct str_traits<str32::value_type> {
		constexpr static str32 defaultTrim() { return U" \t\f\v\n\r"; }
		constexpr static str32 empty() { return U""; }
		constexpr static str32 space() { return U" "; }
		constexpr static str32 bTrue() { return U"true"; }
		constexpr static str32 bFalse() { return U"false"; }
	};


	template<typename OutputType, typename InputType>
	static OutputType lexical_cast(InputType const& value) {
		std::stringstream stream;

		//skip leading whitespace characters on input
		stream.unsetf(std::ios::skipws);

		//set the correct precision
		if (std::numeric_limits<OutputType>::is_specialized) {
			stream.precision(std::numeric_limits<OutputType>::digits10 + 1);
		}
		//		else if (std::numeric_limits<InputType>::is_specialized)
		//		{
		//			stream.precision(std::numeric_limits<InputType>::digits10 + 1);
		//		}

		stream << value;

		OutputType output;
		stream >> output;
		return output;
	}

	template<typename O, typename I>
	O copy_cast(I const& input) {
		O stream;
		for (auto const& current : input)
			stream += typename O::value_type(current);
		return stream;
	}

	//
	// numeric conversions
	//

	str8 to_str8(int8_t val) { return lexical_cast<str8>(val); }
	str8 to_str8(int16_t val) { return lexical_cast<str8>(val); }
	str8 to_str8(int32_t val) { return lexical_cast<str8>(val); }
	str8 to_str8(int64_t val) { return lexical_cast<str8>(val); }
	str8 to_str8(uint8_t val) { return lexical_cast<str8>(val); }
	str8 to_str8(uint16_t val) { return lexical_cast<str8>(val); }
	str8 to_str8(uint32_t val) { return lexical_cast<str8>(val); }
	str8 to_str8(uint64_t val) { return lexical_cast<str8>(val); }
	str8 to_str8(size_t val) { return lexical_cast<str8>(val); }
	str8 to_str8(float val) { return lexical_cast<str8>(val); }
	str8 to_str8(double val) { return lexical_cast<str8>(val); }
	str8 to_str8(bool val) { return val ? str_traits<str8::value_type>::bTrue() : str_traits<str8::value_type>::bFalse(); }

	str32 to_str32(int8_t val) { return copy_cast<str32, str8>(to_str8(val)); }
	str32 to_str32(int16_t val) { return copy_cast<str32, str8>(to_str8(val)); }
	str32 to_str32(int32_t val) { return copy_cast<str32, str8>(to_str8(val)); }
	str32 to_str32(int64_t val) { return copy_cast<str32, str8>(to_str8(val)); }
	str32 to_str32(uint8_t val) { return copy_cast<str32, str8>(to_str8(val)); }
	str32 to_str32(uint16_t val) { return copy_cast<str32, str8>(to_str8(val)); }
	str32 to_str32(uint32_t val) { return copy_cast<str32, str8>(to_str8(val)); }
	str32 to_str32(uint64_t val) { return copy_cast<str32, str8>(to_str8(val)); }
	str32 to_str32(size_t val) { return copy_cast<str32, str8>(to_str8(val)); }
	str32 to_str32(float val) { return copy_cast<str32, str8>(to_str8(val)); }
	str32 to_str32(double val) { return copy_cast<str32, str8>(to_str8(val)); }
	str32 to_str32(bool val) { return val ? str_traits<str32::value_type>::bTrue() : str_traits<str32::value_type>::bFalse(); }

	int32_t str8toi(str8 const& val) { return lexical_cast<int32_t>(val); }
	int64_t str8toi64(str8 const& val) { return lexical_cast<int64_t>(val); }
	uint32_t str8tou(str8 const& val) { return lexical_cast<uint32_t>(val); }
	uint64_t str8tou64(str8 const& val) { return lexical_cast<uint64_t>(val); }
	float str8tof(str8 const& val) { return lexical_cast<float>(val); }
	double str8tod(str8 const& val) { return lexical_cast<double>(val); }
	bool str8tob(str8 const& val) { return (val == "1" || val == str_traits<str8::value_type>::bTrue()); }

	int32_t str32toi(str32 const& val) { return str8toi(copy_cast<str8, str32>(val)); }
	int64_t str32toi64(str32 const& val) { return str8toi64(copy_cast<str8, str32>(val)); }
	uint32_t str32tou(str32 const& val) { return str8tou(copy_cast<str8, str32>(val)); }
	uint64_t str32tou64(str32 const& val) { return str8tou64(copy_cast<str8, str32>(val)); }
	float str32tof(str32 const& val) { return str8tof(copy_cast<str8, str32>(val)); }
	double str32tod(str32 const& val) { return str8tod(copy_cast<str8, str32>(val)); }
	bool str32tob(str32 const& val) { return (val == U"1" || val == str_traits<str32::value_type>::bTrue()); }

	//
	// Trim Implementatiion
	//
	template<typename T>
	static void trimRightImpl(std::basic_string<T>& string, std::basic_string<T> spaces) {

		if (spaces.empty())
			spaces = str_traits<T>::defaultTrim();

		typename std::basic_string<T>::size_type pos = string.find_last_not_of(spaces);
		if (pos != std::basic_string<T>::npos)
			string.erase(pos + 1);
		else
			string.clear();
	}

	template<typename T>
	static void trimLeftImpl(std::basic_string<T>& string, std::basic_string<T> spaces) {

		if (spaces.empty())
			spaces = str_traits<T>::defaultTrim();

		typename std::basic_string<T>::size_type pos = string.find_first_not_of(spaces);
		if (pos != std::basic_string<T>::npos)
			string.erase(0, pos);

		else
			string.clear();
	}

	template<typename T>
	static void trimImpl(std::basic_string<T>& string, std::basic_string<T> const& spaces) {
		trimLeftImpl(string, spaces);
		trimRightImpl(string, spaces);
	}

	template<typename T>
	static std::basic_string<T> trimImpl(TrimType const& trim_type, std::basic_string<T> string, std::basic_string<T> const& spaces) {
		if ((trim_type == TrimType::TrimLeft) || (trim_type == TrimType::TrimBoth))
			trimLeftImpl(string, spaces);

		if ((trim_type == TrimType::TrimRight) || (trim_type == TrimType::TrimBoth))
			trimRightImpl(string, spaces);

		return string;
	}

	void trimRight(str8& string, str8 const& spaces) { trimRightImpl(string, spaces); }
	void trimLeft(str8& string, str8 const& spaces) { trimLeftImpl(string,spaces); }
	void trim(str8& string, str8 const& spaces) { trimImpl(string, spaces); }
	str8 trim(TrimType const& trim_type, str8 const& string, str8 const& spaces) { return trimImpl(trim_type, string, spaces); }

	void trimRight(str32& string, str32 const& spaces) { trimRightImpl(string, spaces); }
	void trimLeft(str32& string, str32 const& spaces) { trimLeftImpl(string, spaces); }
	void trim(str32& string, str32 const& spaces) { trimImpl(string, spaces); }
	str32 trim(TrimType const& trim_type, str32 const& string, str32 const& spaces) { return trimImpl(trim_type, string, spaces); }

	//
	// Split
	//
	template<typename T>
	static std::vector<std::basic_string<T>> splitImpl(std::basic_string<T> const& input, std::basic_string<T> const& delimiter) {
		std::vector<std::basic_string<T>> output;

		if (input.size() != 0) {
			typename std::basic_string<T>::size_type start = 0;

			do
			{
				typename std::basic_string<T>::size_type end = input.find(delimiter, start);
				if (end == std::basic_string<T>::npos)
					end = input.size();

				std::basic_string<T> selection = input.substr(start, end - start);
				if (!selection.empty())
					output.push_back(selection);

				start = end + delimiter.size();

			} while (start < input.size());
		}
		return output;
	}

	template<typename T>
	static std::basic_string<T> extractTokenImpl(std::basic_string<T> const& input, std::basic_string<T> const& delimiter, int token_number) {
		if (input.size() != 0) {
			typename std::basic_string<T>::size_type start = 0;
			typename std::basic_string<T>::size_type end = start;

			bool searching = true;
			do {
				end = input.find(delimiter, start);
				if (end == std::string::npos) {
					searching = false;
					end = input.size();
				}

				if (token_number == 0)
					searching = false;
				
				if (searching)
					start = end + delimiter.size();
				
			} while (searching);

			return input.substr(start, end - start);
		}
		return input;
	}

	std::vector<str8> split(str8 const& input, str8 const& delimiter) { return splitImpl(input, delimiter); }
	std::vector<str32> split(str32 const& input, str32 const& delimiter) { return splitImpl(input, delimiter); }

	str8 extractToken(str8 const& input, str8 const& delimiter, int token_number) { return extractTokenImpl(input, delimiter, token_number); }
	str32 extractToken(str32 const& input, str32 const& delimiter, int token_number) { return extractTokenImpl(input, delimiter, token_number); }

	//
	// Replace
	// 
	template<typename T>
	static void replaceImpl(std::basic_string<T>& target, std::basic_string<T> const& match, std::basic_string<T> const& replace_string) {
		if (match.empty() || target.empty())
			return;

		typename std::basic_string<T>::size_type position = target.find(match);
		while (position != std::basic_string<T>::npos) {
			target.replace(position, match.size(), replace_string);
			position = target.find(match, position + replace_string.size());
		}
	}

	template<typename T>
	static std::basic_string<T> replaceStringImpl(std::basic_string<T> target, std::basic_string<T> const& match, std::basic_string<T> const& replace_string) {
		replaceImpl(target, match, replace_string);
		return target;
	}
	
	void replace(str8& target, str8 const& match, str8 const& replace_string) { replaceImpl(target, match, replace_string); }
	str8 replaceString(str8 target, str8 const& match, str8 const& replace_string) { return replaceStringImpl(target, match, replace_string); }

	void replace(str32& target, str32 const& match, str32 const& replace_string) { replaceImpl(target, match, replace_string); }
	str32 replaceString(str32 target, str32 const& match, str32 const& replace_string) { return replaceStringImpl(target, match, replace_string); }


	//
	// Case Conversions
	//
	template<typename T>
	static void uppercaseImpl(std::basic_string<T>& string) {
		std::transform(string.begin(), string.end(), string.begin(), [](T codepoint) {
			return T(std::toupper(int(codepoint)));
		});
	}

	template<typename T>
	static void lowercaseImpl(std::basic_string<T>& string) {
		std::transform(string.begin(), string.end(), string.begin(), [](T codepoint) {
			return T(std::tolower(int(codepoint)));
		});
	}

	template<typename T>
	static void camelcaseImpl(std::basic_string<T>& string, bool remove_spaces) {
		bool active = true;

		for (size_t i = 0; i < string.size(); i++) {
			if (std::isalpha(string[i])) {
				if (active) {
					string[i] = T(std::toupper(string[i]));
					active = false;
				} else {
					string[i] = T(std::tolower(string[i]));
				}
			} else if (string[i] == T(' ')) {
				active = true;
			}
		}

		if (remove_spaces) 
			replace(string, str_traits<T>::space(), str_traits<T>::empty());
	}

	template<typename T>
	static std::basic_string<T> convertCaseImpl(CaseType const& case_type, std::basic_string<T> string) {

		if (case_type == CaseType::Uppercase)
			uppercase(string);
		else if (case_type == CaseType::Lowercase)
			lowercase(string);
		else if (case_type == CaseType::CamelCase)
			camelcase(string);

		return string;
	}

	void uppercase(str8& string) { uppercaseImpl(string); }
	void lowercase(str8& string) { lowercaseImpl(string); }
	void camelcase(str8& string, bool remove_spaces) { camelcaseImpl(string, remove_spaces); }
	str8 convertCase(CaseType const& case_type, str8 const& string) { return convertCaseImpl(case_type, string); }

	void uppercase(str32& string) { uppercaseImpl(string); }
	void lowercase(str32& string) { lowercaseImpl(string); }
	void camelcase(str32& string, bool remove_spaces) { camelcaseImpl(string, remove_spaces); }
	str32 convertCase(CaseType const& case_type, str32 const& string) { return convertCaseImpl(case_type, string); }

	//
	// Contains
	//
	template<typename T>
	bool startsWithImpl(std::basic_string<T> const& string, std::basic_string<T> const& prefix) {
		return string.find(prefix) == typename std::basic_string<T>::size_type(0);
	}

	template<typename T>
	bool endsWithImpl(std::basic_string<T> const& string, std::basic_string<T> const& sufix) {
		if (string.size() < sufix.size())
			return false;
		return string.rfind(sufix) == (string.size() - sufix.size());
	}

	template<typename T>
	bool containsTexImpl(std::basic_string<T> const& string, std::basic_string<T> const& text) {
		return string.find(text) != std::string::npos;
	}

	bool startsWith(str8 const& string, str8 const& prefix) { return startsWithImpl(string, prefix); }
	bool endsWith(str8 const& string, str8 const& sufix) { return endsWithImpl(string, sufix); }
	bool containsText(str8 const& string, str8 const& text) { return containsTexImpl(string, text); }

	bool startsWith(str32 const& string, str32 const& prefix) { return startsWithImpl(string, prefix); }
	bool endsWith(str32 const& string, str32 const& sufix) { return endsWithImpl(string, sufix); }
	bool containsText(str32 const& string, str32 const& text) { return containsTexImpl(string, text); }


	//
	// Number Checks
	//
	template<typename T>
	bool isIntegerImpl(std::basic_string<T> const& string) {
		if (string.empty())
			return false;

		typename std::basic_string<T>::const_iterator i = string.begin();
		if (std::isdigit(string[0]) == 0) {
			if ((string[0] != '-') && (string[0] != '+')) {
				return false;
			} else {
				if (++i == string.end())
					return false;
			}
		}

		while (i != string.end()) {
			if (std::isdigit(*i) == 0) 
				return false;
			++i;
		}

		return true;
	}

	template<typename T>
	bool isRealImpl(std::basic_string<T> const& string) {
		if (string.empty())
			return false;

		typename std::basic_string<T>::const_iterator i = string.begin();
		bool dot_found = false;

		if (std::isdigit(string[0]) == 0) {
			if ((string[0] != '-') && (string[0] != '.') && (string[0] != '+')) {
				return false;
			} else {
				if (string[0] == '.')
					dot_found = true;

				if (++i == string.end())
					return false;
			}
		}

		while (i != string.end()) {
			if (std::isdigit(*i) == 0) {
				if (*i == '.') {
					if (dot_found)
						return false;
					else
						dot_found = true;
				} else {
					return false;
				}
			}
			++i;
		}

		return dot_found;
	}

	template<typename T>
	bool isBooleanImpl(std::basic_string<T> const& string) {
		if (isInteger(string))
			return ((string.size() == 1) && ((string[0] == '0') || (string[0] == '1')));
		else
			return ((string == str_traits<T>::bTrue()) || (string == str_traits<T>::bFalse()));
	}

	bool isNumber(str8 const& string) { return (isInteger(string) || isReal(string)); }
	bool isInteger(str8 const& string) { return isIntegerImpl(string); }
	bool isReal(str8 const& string) { return isRealImpl(string); }
	bool isBoolean(str8 const& string) { return isBooleanImpl(string); }

	bool isNumber(str32 const& string) { return (isInteger(string) || isReal(string)); }
	bool isInteger(str32 const& string) { return isIntegerImpl(string); }
	bool isReal(str32 const& string) { return isRealImpl(string); }
	bool isBoolean(str32 const& string) { return isBooleanImpl(string); }

	//
	// Equals
	//
	template<typename T>
	static bool equalsImpl(std::basic_string<T> const& lhs, std::basic_string<T> const& rhs, bool case_sensitive) {

		if (case_sensitive)
			return lhs == rhs;

		// case insensitive
		if (lhs.size() != rhs.size())
			return false;

		typename std::basic_string<T>::const_iterator litr = lhs.begin();
		typename std::basic_string<T>::const_iterator ritr = rhs.begin();

		while (litr != lhs.end()) {
			if (std::tolower(*litr) != std::tolower(*ritr))
				return false;

			++litr;
			++ritr;
		}
		return true;
	}

	bool equals(str8 const& lhs, str8 const& rhs, bool case_sensitive) { return equalsImpl(lhs, rhs, case_sensitive); }
	bool equals(str32 const& lhs, str32 const& rhs, bool case_sensitive) { return equalsImpl(lhs, rhs, case_sensitive); }

	//
	// debug string
	//
	template<typename T, typename C>
	static str8 debugStrImpl(std::basic_string<T> const& input) {
		std::stringstream stream;
		for (size_t i = 0; i != input.size(); ++i) {
			uint32_t value = (C)input[i];
			if (value < 0x7f)
				stream << (unsigned char) input[i];
			 else 
				stream << "[0x" << std::hex << std::uppercase << std::setfill('0') << value << "]";
		}
		return stream.str();
	}

	str8 debugStr(str8 const& input) { return debugStrImpl<str8::value_type, unsigned char>(input); }
	str8 debugStr(str16 const& input)  { return debugStrImpl<str16::value_type, unsigned short>(input); }
	str8 debugStr(str32 const& input) { return debugStrImpl<str32::value_type, unsigned int>(input); }


	//
	// utf conversions
	//
	// https://www.babelstone.co.uk/Unicode/whatisit.html
	// https://opensource.apple.com/source/tidy/tidy-2.2/tidy/src/utf8.c.auto.html
	// https://github.com/nu774/libid3tag/blob/master/utf16.c

	constexpr uint32_t kUTF8ByteSwapNotAChar = 0xFFFE;
	constexpr uint32_t kUTF8NotAChar = 0xFFFF;
	constexpr uint32_t kMaxUTF8FromUCS4 = 0x10FFFF;
	constexpr uint32_t kUTF16LowSurrogateBegin = 0xD800;
	constexpr uint32_t kUTF16LowSurrogateEnd = 0xDBFF;
	constexpr uint32_t kUTF16HighSurrogateBegin = 0xDC00;
	constexpr uint32_t kUTF16HighSurrogateEnd = 0xDFFF;

	str32 decodeUtf8(uint8_t const* input, size_t size, str32 const& replacement) {
		if (!input || !size) return str32();

		std::vector<str32::value_type> output;
		output.reserve(size);

		size_t cursor = 0;

		bool end_found = false;

		while ((cursor < size) && !end_found) {
			bool error = false;
			uint8_t ch = input[cursor];
			uint32_t codepoint;
			uint32_t bytes;

			if (ch <= 0x7F) { // 0XXX XXXX one byte 
				codepoint = ch;
				bytes = 1;
			} else if ((ch & 0xE0) == 0xC0) { //110X XXXX  two bytes 
				codepoint = ch & 31;
				bytes = 2;
			} else if ((ch & 0xF0) == 0xE0) { // 1110 XXXX  three bytes
				codepoint = ch & 15;
				bytes = 3;
			} else if ((ch & 0xF8) == 0xF0) { // 1111 0XXX  four bytes
				codepoint = ch & 7;
				bytes = 4;
			} else if ((ch & 0xFC) == 0xF8) { // 1111 10XX  five bytes 
				codepoint = ch & 3;
				bytes = 5;
				error = true;
			} else if ((ch & 0xFE) == 0xFC) {  // 1111 110X  six bytes
				codepoint = ch & 1;
				bytes = 6;
				error = true;
			} else { // not a valid first byte of a UTF-8 sequence
				codepoint = ch;
				bytes = 1;
				error = true;
			}

			if ((cursor + bytes) <= size) {
				// compute codepoint
				for (int i = 0; i < bytes - 1; ++i) {
					uint8_t next = input[cursor + i + 1];

					if (!next || (next & 0xC0) != 0x80) {
						// null terminator found
						end_found = true;
						break;
					}
					codepoint = (codepoint << 6) | (next & 0x3F);
				}

				if (!error && ((codepoint == kUTF8ByteSwapNotAChar) || (codepoint == kUTF8NotAChar)))
					error = true;

				if (!error && (codepoint > kMaxUTF8FromUCS4))
					error = true;


				if (error) {
					for (auto current : replacement)
						output.push_back(current);
				} else {
					output.push_back(codepoint);
				}
				
				cursor += bytes;
			} else {
				// erroneous utf data
				end_found = true;
			}
		}

		if (output.empty())
			return str32();

		return str32(output.begin(), output.end());
	}

	str8 encodeUtf8(str32 const& input) {
		if (input.empty()) return str8();

		uint8_t buffer[10] = { 0 };
		using CT = uint8_t;

		std::vector<str8::value_type> output;
		output.reserve(input.size());
;
		for (size_t i = 0; i != input.size(); ++i) {
			int32_t c = input[i];
			uint32_t bytes;
			bool error = false;

			if (c <= 0x7F) { // 0XXX XXXX one byte 
				buffer[0] = (CT)c;
				bytes = 1;
			} else if (c <= 0x7FF) { // 110X XXXX  two bytes
				buffer[0] = (CT)(0xC0 | (c >> 6));
				buffer[1] = (CT)(0x80 | (c & 0x3F));
				bytes = 2;
			} else if (c <= 0xFFFF) { // 1110 XXXX  three bytes 
				buffer[0] = (CT)(0xE0 | (c >> 12));
				buffer[1] = (CT)(0x80 | ((c >> 6) & 0x3F));
				buffer[2] = (CT)(0x80 | (c & 0x3F));
				bytes = 3;
				if (c == kUTF8ByteSwapNotAChar || c == kUTF8NotAChar) error = true;
			} else if (c <= 0x1FFFFF) { // 1111 0XXX  four bytes
				buffer[0] = (CT)(0xF0 | (c >> 18));
				buffer[1] = (CT)(0x80 | ((c >> 12) & 0x3F));
				buffer[2] = (CT)(0x80 | ((c >> 6) & 0x3F));
				buffer[3] = (CT)(0x80 | (c & 0x3F));
				bytes = 4;
				if (c > kMaxUTF8FromUCS4) error = true;
			} else if (c <= 0x3FFFFFF) { // 1111 10XX  five bytes
				buffer[0] = (CT)(0xF8 | (c >> 24));
				buffer[1] = (CT)(0x80 | (c >> 18));
				buffer[2] = (CT)(0x80 | ((c >> 12) & 0x3F));
				buffer[3] = (CT)(0x80 | ((c >> 6) & 0x3F));
				buffer[4] = (CT)(0x80 | (c & 0x3F));
				bytes = 5;
				error = true;
			} else if (c <= 0x7FFFFFFF) { // 1111 110X  six bytes
				buffer[0] = (CT)(0xFC | (c >> 30));
				buffer[1] = (CT)(0x80 | ((c >> 24) & 0x3F));
				buffer[2] = (CT)(0x80 | ((c >> 18) & 0x3F));
				buffer[3] = (CT)(0x80 | ((c >> 12) & 0x3F));
				buffer[4] = (CT)(0x80 | ((c >> 6) & 0x3F));
				buffer[5] = (CT)(0x80 | (c & 0x3F));
				bytes = 6;
				error = true;
			}
			else
				error = true;


			if (!error) {
				for (int ix = 0; ix < bytes; ++ix)
					output.push_back(buffer[ix]);
			}
		}

		if (output.empty())
			return str8();

		return str8(output.begin(), output.end());
	}

	str32 decodeUtf16(uint16_t const* input, size_t size) {
		if (!input || !size) return str32();

		using CT = str32::value_type;

		std::vector<CT> output;
		output.reserve(size);

		size_t cursor = 0;
		bool end_found = false;
		while ((cursor < size) && !end_found) {
			bool error = false;
			CT ch = (CT)input[cursor];
			size_t used = 1;

			if ((ch >= kUTF16LowSurrogateBegin && ch <= kUTF16LowSurrogateEnd) && (cursor < (size - 1))) {
				CT ch1 = (CT)input[cursor + 1];
				if (ch1 >= kUTF16HighSurrogateBegin && ch1 <= kUTF16HighSurrogateEnd) {
					ch = (((ch & 0x03ff) << 10) | ((ch1 & 0x03ff) << 0)) + 0x00010000;
					used = 2;
				}
			}
	
			output.push_back(ch);
			cursor += used;
		}

		return str32(output.begin(), output.end());
	}

	str16 encodeUtf16(str32 const& input) {
		if (input.empty()) return str16();

		std::vector<str16::value_type> output;
		output.reserve(input.size());

		for (size_t i = 0; i != input.size(); ++i) {
			int32_t c = input[i];
			if (c < 0x00010000L) {
				output.push_back(c);
			} else if (c < 0x00110000L) {
				c -= 0x00010000L;

				output.push_back( ((c >> 10) & 0x3ff) | kUTF16LowSurrogateBegin );
				output.push_back( ((c >>  0) & 0x3ff) | kUTF16HighSurrogateBegin );
			}
		}

		return str16(output.begin(), output.end());
	}

	str8 u8To8(std::u8string const& input) { return str8(reinterpret_cast<str8::value_type const*>(input.data())); }
	str32 str8To32(str8 const& input) { return decodeUtf8((uint8_t*)input.data(), input.size()); }
	str8 str32To8(str32 const& input) { return encodeUtf8(input); }
	str32 str16To32(str16 const& input) { return decodeUtf16((uint16_t*)input.data(), input.size()); }
	str16 str32To16(str32 const& input) { return encodeUtf16(input); }



	//
	// glob
	//
	// based on - https://github.com/tsoding/glob.h
	//

	template<typename T>
	static bool globImpl(T const* pattern, T const* text) {
		if (!pattern && !text) return true;
		if (!pattern || !text) return false;
		
		T const* p_cursor = pattern;
		T const* t_cursor = text;

		T const* scanning_start = nullptr;
		bool matched_option = false;
		bool inverted = false;

		while (*t_cursor && *p_cursor) {

			// the option
			if (scanning_start) {
				if (*p_cursor == ']') {
					scanning_start = nullptr;
					if ((matched_option && !inverted) || 
					    (!matched_option && inverted)) {
						t_cursor += 1;
					} else {	
						break;
					}
				} else if ( // [a-z]
					(*p_cursor == '-') && (p_cursor - scanning_start > 0) && 
				    *(p_cursor + 1) && (*(p_cursor + 1) != ']') ) { 
					if (*t_cursor >= *(p_cursor - 1) && *t_cursor <= *(p_cursor + 1)) {
						matched_option = true;
					}
				} else if (*t_cursor == *p_cursor) {
					matched_option = true;
				} 
				p_cursor += 1;
			} else if (*p_cursor == '[') { // start scanning option []
				matched_option = false;
				inverted = false; 
				p_cursor += 1;

				if (*p_cursor == '!') {
					inverted = true; 
					p_cursor += 1;
				}

				if (*p_cursor == ']') {
					if (*t_cursor == *p_cursor)
						matched_option = true;
					
					p_cursor += 1;
				}
				scanning_start = p_cursor;
			} 


			else if (*p_cursor == '\\') { // escaping
				p_cursor += 1;
			}
			
			
			// matching * character
			else if (*p_cursor == '*') { 
                if (glob(p_cursor + 1, t_cursor)) return true; // check if matching without the start works for the rest of the text
                t_cursor += 1; // discard a character
            } 
			
			// matching the ? character
			else if (*p_cursor == '?') { 
                t_cursor += 1;
                p_cursor += 1;
            } 
			
			// matching exactly a character
			else if (*t_cursor == *p_cursor) { 
				t_cursor += 1;
				p_cursor += 1;
			} else {
				break;
			}
		}

        if (*t_cursor == 0) {
            while (*p_cursor == '*') p_cursor += 1;
            return (*p_cursor == 0);
        }
        
        return false;
	}

	template<typename T>
	std::vector<T> globMultiImpl(T const& pattern, std::vector<T> const& text) {
		std::vector<T> output;
		
		for (auto const& current : text)
			if (glob(pattern, current))
				output.push_back(current);

		return output;
	}

	bool glob(str8 const& pattern, str8 const& text)	{ return globImpl(pattern.data(), text.data()); }
	bool glob(str32 const& pattern, str32 const& text)	{ return globImpl(pattern.data(), text.data()); }

	std::vector<str8> glob(str8 const& pattern, std::vector<str8> const& text) { return globMultiImpl(pattern, text); }
	std::vector<str32> glob(str32 const& pattern, std::vector<str32> const& text) { return globMultiImpl(pattern, text); }



    //
    // Levenshtein distance
    //
    static size_t minimum(size_t a, size_t b) { return (a < b) ? a : b; }
    static size_t minimum(size_t a, size_t b, size_t c) { return minimum(minimum(a, b), c); }

    template<typename T>
    static size_t levenshteinDistanceImpl(std::basic_string<T> const& source, std::basic_string<T> const& target) {
        if (source.empty()) return target.size();
        else if (target.empty()) return source.size();
        
        std::vector<std::vector<size_t>> m(source.size() + 1, std::vector<size_t>(target.size() + 1, 0));
        
        // source prefixes can be transformed into empty string by dropping all characters
        for (size_t i = 0; i != source.size() + 1; ++i) m[i][0] = i;
            
        // target prefixes can be reached from empty source prefix by inserting every character
        for (size_t j = 0; j != target.size() + 1; ++j) m[0][j] = j;
                
        for (size_t t = 0; t != target.size(); ++t) {
            for (size_t s = 0; s != source.size(); ++s) {
                size_t substitutionCost = (source[s] == target[t]) ? 0 : 1;
                
                size_t j = t + 1;
                size_t i = s + 1;
                m[i][j] = minimum(m[i - 1][j    ] + 1,                 // deletion
                                  m[i    ][j - 1] + 1,                 // insertion
                                  m[i - 1][j - 1] + substitutionCost); // substitution
            }
        }
                
        return m[source.size()][target.size()];
    }
    
    size_t levenshteinDistance(str8 const& source, str8 const& target) { return levenshteinDistanceImpl(source, target); }
    size_t levenshteinDistance(str32 const& source, str32 const& target) { return levenshteinDistanceImpl(source, target); }


    template<typename T>
    std::vector<T> similiarSortImpl(T const& source, std::vector<T> const& text) {
        std::vector<T> output = text;
        
        sort(output.begin(), output.end(), [&source](T const& a, T const& b) {
            size_t a_score = levenshteinDistance(source, a);
            size_t b_score = levenshteinDistance(source, b);
            return a_score < b_score;
        });
        
        return output;
    }

    std::vector<str8> similiarSort(str8 const& source, std::vector<str8> const& text) { return similiarSortImpl(source, text); }
    std::vector<str32> similiarSort(str32 const& source, std::vector<str32> const& text) { return similiarSortImpl(source, text); }
}
