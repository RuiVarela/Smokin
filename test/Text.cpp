#include <UnitTest.hpp>
#include <Text.hpp>
#include <iostream>

using namespace smk;

TEST_BEGIN(TextTrim)
{
	{
		std::string message = "   hello   ";

		std::string processed = message;
		trimRight(processed);
		TEST_FAIL_NOT_EQUAL("   hello", processed);
		TEST_FAIL_NOT_EQUAL(processed, "   hello");
		TEST_FAIL_NOT_EQUAL("   hello", "   hello");
		TEST_FAIL_NOT_EQUAL(processed, processed);

		processed = message;
		trimLeft(processed);
		TEST_FAIL_NOT_EQUAL("hello   ", processed);

		processed = message;
		trim(processed);
		TEST_FAIL_NOT_EQUAL("hello", processed);

		TEST_FAIL_NOT_EQUAL(trim(TrimType::TrimBoth, "super mario   "), "super mario");
	}

	{
		smk::str8 message = "   hello   ";

		smk::str8 processed = message;
		trimRight(processed);
		TEST_FAIL_NOT_EQUAL("   hello", processed);
		TEST_FAIL_NOT_EQUAL(processed, "   hello");

		processed = message;
		trimLeft(processed);
		TEST_FAIL_NOT_EQUAL("hello   ", processed);

		processed = message;
		trim(processed);
		TEST_FAIL_NOT_EQUAL("hello", processed);

		TEST_FAIL_NOT_EQUAL(trim(TrimType::TrimBoth, "super mario   "), "super mario");
	}


	{
		smk::str32 message = U"   hello   ";

		smk::str32 processed = message;
		trimRight(processed);
		TEST_FAIL_NOT_EQUAL(U"   hello", processed);

		processed = message;
		trimLeft(processed);
		TEST_FAIL_NOT_EQUAL(U"hello   ", processed);

		processed = message;

		trim(processed);
		TEST_FAIL_NOT_EQUAL(U"hello", processed);

		TEST_FAIL_NOT_EQUAL(trim(TrimType::TrimBoth, U"super mario   "), U"super mario");
	}	
}
TEST_END(TextTrim)


TEST_BEGIN(TextNumericConversions)
{
	{
		TEST_FAIL_NOT_EQUAL("2", to_str8(2.0f));
		TEST_FAIL_NOT_EQUAL("2.55", to_str8(2.55f));
		TEST_FAIL_NOT_EQUAL("3.56112", to_str8(3.56112));

		TEST_FAIL_NOT_EQUAL("45", to_str8(45));
		TEST_FAIL_NOT_EQUAL("-76", to_str8(-76));

		TEST_FAIL_NOT_EQUAL(3.55, str8tod("3.55"));
		TEST_FAIL_NOT_EQUAL(3.55f, str8tof("3.55"));

		TEST_FAIL_NOT_EQUAL(1.0, str8tod("1.000000"));
		TEST_FAIL_NOT_EQUAL(1.0f, str8tof("1.000000"));
		TEST_FAIL_NOT_EQUAL(1.0f, str8tof("1"));
		TEST_FAIL_NOT_EQUAL(1.0f, str8tof("1.0"));
		TEST_FAIL_NOT_EQUAL(2.01f, str8tof("2.01"));

		TEST_FAIL_NOT_EQUAL(1, str8toi("1"));
		TEST_FAIL_NOT_EQUAL(15, str8toi("15"));
		TEST_FAIL_NOT_EQUAL(35678, str8toi("35678"));
		TEST_FAIL_NOT_EQUAL(-35678, str8toi("-35678"));

		TEST_FAIL_NOT_EQUAL(true, str8tob("1"));
		TEST_FAIL_NOT_EQUAL(true, str8tob("true"));

		TEST_FAIL_NOT_EQUAL(false, str8tob("0"));
		TEST_FAIL_NOT_EQUAL(false, str8tob("false"));
		TEST_FAIL_NOT_EQUAL(false, str8tob("asd"));
	}

	{
		TEST_FAIL_NOT_EQUAL(U"2", to_str32(2.0f));
		TEST_FAIL_NOT_EQUAL(U"2.55", to_str32(2.55f));
		TEST_FAIL_NOT_EQUAL(U"3.56112", to_str32(3.56112));

		TEST_FAIL_NOT_EQUAL(U"45", to_str32(45));
		TEST_FAIL_NOT_EQUAL(U"-76", to_str32(-76));

		TEST_FAIL_NOT_EQUAL(3.55, str32tod(U"3.55"));
		TEST_FAIL_NOT_EQUAL(3.55f, str32tof(U"3.55"));

		TEST_FAIL_NOT_EQUAL(1.0, str32tod(U"1.000000"));
		TEST_FAIL_NOT_EQUAL(1.0f, str32tof(U"1.000000"));
		TEST_FAIL_NOT_EQUAL(1.0f, str32tof(U"1"));
		TEST_FAIL_NOT_EQUAL(1.0f, str32tof(U"1.0"));
		TEST_FAIL_NOT_EQUAL(2.01f, str32tof(U"2.01"));

		TEST_FAIL_NOT_EQUAL(1, str32toi(U"1"));
		TEST_FAIL_NOT_EQUAL(15, str32toi(U"15"));
		TEST_FAIL_NOT_EQUAL(35678, str32toi(U"35678"));
		TEST_FAIL_NOT_EQUAL(-35678, str32toi(U"-35678"));

		TEST_FAIL_NOT_EQUAL(true, str32tob(U"1"));
		TEST_FAIL_NOT_EQUAL(true, str32tob(U"true"));

		TEST_FAIL_NOT_EQUAL(false, str32tob(U"0"));
		TEST_FAIL_NOT_EQUAL(false, str32tob(U"false"));
		TEST_FAIL_NOT_EQUAL(false, str32tob(U"asd"));
	}
}
TEST_END(TextNumericConversions)


TEST_BEGIN(TextSplit)
{
	{
		std::string message = "hello c++, and other worlds++one++two++three++";
		auto elements = split(message, " ");
		TEST_FAIL_NOT_EQUAL(5, (int)elements.size());
		TEST_FAIL_NOT_EQUAL("hello", elements[0]);
		TEST_FAIL_NOT_EQUAL("c++,", elements[1]);
		TEST_FAIL_NOT_EQUAL("and", elements[2]);
		TEST_FAIL_NOT_EQUAL("other", elements[3]);
		TEST_FAIL_NOT_EQUAL("worlds++one++two++three++", elements[4]);

		elements = split(message, ",");
		TEST_FAIL_NOT_EQUAL(2, (int)elements.size());
		TEST_FAIL_NOT_EQUAL("hello c++", elements[0]);
		TEST_FAIL_NOT_EQUAL(" and other worlds++one++two++three++", elements[1]);

		elements = split(message, "++");
		TEST_FAIL_NOT_EQUAL(5, (int)elements.size());
		TEST_FAIL_NOT_EQUAL("hello c", elements[0]);
		TEST_FAIL_NOT_EQUAL(", and other worlds", elements[1]);
		TEST_FAIL_NOT_EQUAL("one", elements[2]);
		TEST_FAIL_NOT_EQUAL("two", elements[3]);
		TEST_FAIL_NOT_EQUAL("three", elements[4]);

		elements = split(message, "!");
		TEST_FAIL_NOT_EQUAL(1, (int)elements.size());

		TEST_FAIL_NOT_EQUAL("hello c++", extractToken(message, ",", 0));
		TEST_FAIL_NOT_EQUAL(" and other worlds++one++two++three++", extractToken(message, ",", 1));
		TEST_FAIL_NOT_EQUAL("hello c++, and ", extractToken(message, "other worlds++", 0));
		TEST_FAIL_NOT_EQUAL("one++two++three++", extractToken(message, "other worlds++", 1));
	}

	{
		str32 message = U"hello c++, and other worlds++one++two++three++";
		auto elements = split(message, U" ");
		TEST_FAIL_NOT_EQUAL(5, (int)elements.size());
		TEST_FAIL_NOT_EQUAL(U"hello", elements[0]);
		TEST_FAIL_NOT_EQUAL(U"c++,", elements[1]);
		TEST_FAIL_NOT_EQUAL(U"and", elements[2]);
		TEST_FAIL_NOT_EQUAL(U"other", elements[3]);
		TEST_FAIL_NOT_EQUAL(U"worlds++one++two++three++", elements[4]);

		elements = split(message, U",");
		TEST_FAIL_NOT_EQUAL(2, (int)elements.size());
		TEST_FAIL_NOT_EQUAL(U"hello c++", elements[0]);
		TEST_FAIL_NOT_EQUAL(U" and other worlds++one++two++three++", elements[1]);

		elements = split(message, U"++");
		TEST_FAIL_NOT_EQUAL(5, (int)elements.size());
		TEST_FAIL_NOT_EQUAL(U"hello c", elements[0]);
		TEST_FAIL_NOT_EQUAL(U", and other worlds", elements[1]);
		TEST_FAIL_NOT_EQUAL(U"one", elements[2]);
		TEST_FAIL_NOT_EQUAL(U"two", elements[3]);
		TEST_FAIL_NOT_EQUAL(U"three", elements[4]);

		elements = split(message, U"!");
		TEST_FAIL_NOT_EQUAL(1, (int)elements.size());

		TEST_FAIL_NOT_EQUAL(U"hello c++", extractToken(message, U",", 0));
		TEST_FAIL_NOT_EQUAL(U" and other worlds++one++two++three++", extractToken(message, U",", 1));
		TEST_FAIL_NOT_EQUAL(U"hello c++, and ", extractToken(message, U"other worlds++", 0));
		TEST_FAIL_NOT_EQUAL(U"one++two++three++", extractToken(message, U"other worlds++", 1));
	}
}
TEST_END(TextSplit)


TEST_BEGIN(TextReplace)
{
	{		
		str8 message = "one two three";
		replace(message, "two", "four");
		TEST_FAIL_NOT_EQUAL("one four three", message);

		replace(message, "four", "");
		TEST_FAIL_NOT_EQUAL("one  three", message);

		TEST_FAIL_NOT_EQUAL("-A-B-C-", replaceString(" one A one B one C one ", " one ", "-"));
	}

	{
		str32 message = U"one two three";
		replace(message, U"two", U"four");
		TEST_FAIL_NOT_EQUAL(U"one four three", message);

		replace(message, U"four", U"");
		TEST_FAIL_NOT_EQUAL(U"one  three", message);

		TEST_FAIL_NOT_EQUAL(U"-A-B-C-", replaceString(U" one A one B one C one ", U" one ", U"-"));
	}
}
TEST_END(TextReplace)


TEST_BEGIN(TextCase)
{
	{
		str8 message = "one two three";
		uppercase(message);
		TEST_FAIL_NOT_EQUAL("ONE TWO THREE", message);

		lowercase(message);
		TEST_FAIL_NOT_EQUAL("one two three", message);

		camelcase(message);
		TEST_FAIL_NOT_EQUAL("One Two Three", message);

		camelcase(message, true);
		TEST_FAIL_NOT_EQUAL("OneTwoThree", message);
	}

	{
		str32 message = U"one two three";
		uppercase(message);
		TEST_FAIL_NOT_EQUAL(U"ONE TWO THREE", message);

		lowercase(message);
		TEST_FAIL_NOT_EQUAL(U"one two three", message);

		camelcase(message);
		TEST_FAIL_NOT_EQUAL(U"One Two Three", message);

		camelcase(message, true);
		TEST_FAIL_NOT_EQUAL(U"OneTwoThree", message);
	}
}
TEST_END(TextCase)


TEST_BEGIN(TextContains)
{
	{
		TEST_ASSERT(startsWith("prefix$body", "prefix"));
		TEST_ASSERT(!startsWith("aprefix$body", "prefix"));
		TEST_ASSERT(startsWith("$prefix$body", "$prefix"));
		TEST_ASSERT(!startsWith(" prefix$body", "$prefix"));
		TEST_ASSERT(!startsWith(" prefix$body", "prefix"));
		TEST_ASSERT(!startsWith("", "prefix"));

		TEST_ASSERT(endsWith("prefix$body$sufix", "sufix"));
		TEST_ASSERT(!endsWith("aprefix$sufixc", "sufix"));
		TEST_ASSERT(endsWith("$prefix$bodydsufix$", "sufix$"));
		TEST_ASSERT(!endsWith(" prefix$body ", "prefix"));
		TEST_ASSERT(endsWith(" prefix$body prefix ", "prefix "));
		TEST_ASSERT(!endsWith("", "prefix "));

		TEST_ASSERT(containsText("super mario world", "mario"));
		TEST_ASSERT(containsText("super mario world", "super"));
		TEST_ASSERT(containsText("super mario world", "world"));
		TEST_ASSERT(!containsText("super mario world", "luigi"));
	}

	{
		TEST_ASSERT(startsWith(U"prefix$body", U"prefix"));
		TEST_ASSERT(!startsWith(U"aprefix$body", U"prefix"));
		TEST_ASSERT(startsWith(U"$prefix$body", U"$prefix"));
		TEST_ASSERT(!startsWith(U" prefix$body", U"$prefix"));
		TEST_ASSERT(!startsWith(U" prefix$body", U"prefix"));
		TEST_ASSERT(!startsWith(U"", U"prefix"));

		TEST_ASSERT(endsWith(U"prefix$body$sufix", U"sufix"));
		TEST_ASSERT(!endsWith(U"aprefix$sufixc", U"sufix"));
		TEST_ASSERT(endsWith(U"$prefix$bodydsufix$", U"sufix$"));
		TEST_ASSERT(!endsWith(U" prefix$body ", U"prefix"));
		TEST_ASSERT(endsWith(U" prefix$body prefix ", U"prefix "));
		TEST_ASSERT(!endsWith(U"", U"prefix "));

		TEST_ASSERT(containsText(U"super mario world", U"mario"));
		TEST_ASSERT(containsText(U"super mario world", U"super"));
		TEST_ASSERT(containsText(U"super mario world", U"world"));
		TEST_ASSERT(!containsText(U"super mario world", U"luigi"));
	}
}
TEST_END(TextContains)

TEST_BEGIN(TextNumberChecks)
{
	{
		TEST_ASSERT(isInteger("10"));
		TEST_ASSERT(isInteger("112355"));
		TEST_ASSERT(isInteger("776213810"));
		TEST_ASSERT(isInteger("+15"));
		TEST_ASSERT(isInteger("-10"));
		TEST_ASSERT(!isInteger("+15+"));
		TEST_ASSERT(!isInteger("-10-"));
		TEST_ASSERT(!isInteger("+"));
		TEST_ASSERT(!isInteger("-"));
		TEST_ASSERT(!isInteger("1-10"));
		TEST_ASSERT(!isInteger("1+10"));
		TEST_ASSERT(!isInteger("a110"));
		TEST_ASSERT(!isInteger("1a10"));
		TEST_ASSERT(!isInteger("110a"));
		TEST_ASSERT(!isInteger("1.10"));

		TEST_ASSERT(isReal("1.0"));
		TEST_ASSERT(isReal("21."));
		TEST_ASSERT(isReal(".13"));
		TEST_ASSERT(isReal("123.49"));
		TEST_ASSERT(isReal("456.78"));
		TEST_ASSERT(isReal("+12456.78"));
		TEST_ASSERT(isReal("-12456.78"));
		TEST_ASSERT(isReal("123110.123410"));
		TEST_ASSERT(!isReal("--"));
		TEST_ASSERT(!isReal("+"));
		TEST_ASSERT(!isReal("."));
		TEST_ASSERT(!isReal("1"));
		TEST_ASSERT(!isReal("10"));
		TEST_ASSERT(!isReal("a110"));
		TEST_ASSERT(!isReal("1a10"));
		TEST_ASSERT(!isReal("110a.0"));
		TEST_ASSERT(!isReal("1+10.0"));
		TEST_ASSERT(!isReal("1231-10.0"));
		TEST_ASSERT(!isReal("*123110.123410"));
		TEST_ASSERT(!isReal("1.23110.123410"));

		TEST_ASSERT(isBoolean("1"));
		TEST_ASSERT(isBoolean("0"));
		TEST_ASSERT(isBoolean("false"));
		TEST_ASSERT(isBoolean("true"));
		TEST_ASSERT(!isBoolean("False"));
		TEST_ASSERT(!isBoolean("True"));
		TEST_ASSERT(!isBoolean("10"));
		TEST_ASSERT(!isBoolean("112355"));
		TEST_ASSERT(!isBoolean("776213810"));
		TEST_ASSERT(!isBoolean("+15"));
		TEST_ASSERT(!isBoolean("-10"));
		TEST_ASSERT(!isBoolean("+15+"));
		TEST_ASSERT(!isBoolean("-10-"));
		TEST_ASSERT(!isBoolean("+"));
		TEST_ASSERT(!isBoolean("-"));
		TEST_ASSERT(!isBoolean("1-10"));
		TEST_ASSERT(!isBoolean("1+10"));
		TEST_ASSERT(!isBoolean("a110"));
		TEST_ASSERT(!isBoolean("1a10"));
		TEST_ASSERT(!isBoolean("110a"));
		TEST_ASSERT(!isBoolean("1.10"));

		TEST_ASSERT(isNumber("10"));
		TEST_ASSERT(isNumber("112355"));
		TEST_ASSERT(isNumber("776213810"));
		TEST_ASSERT(isNumber("+15"));
		TEST_ASSERT(isNumber("-10"));
		TEST_ASSERT(isNumber("1.0"));
		TEST_ASSERT(isNumber("21."));
		TEST_ASSERT(isNumber(".13"));
		TEST_ASSERT(isNumber("123.49"));
		TEST_ASSERT(isNumber("456.78"));
		TEST_ASSERT(isNumber("+12456.78"));
		TEST_ASSERT(isNumber("-12456.78"));
		TEST_ASSERT(isNumber("123110.123410"));
		TEST_ASSERT(!isNumber("+15+"));
		TEST_ASSERT(!isNumber("-10-"));
		TEST_ASSERT(!isNumber("+"));
		TEST_ASSERT(!isNumber("-"));
		TEST_ASSERT(!isNumber("1-10"));
		TEST_ASSERT(!isNumber("1+10"));
		TEST_ASSERT(!isNumber("a110"));
		TEST_ASSERT(!isNumber("1a10"));
		TEST_ASSERT(!isNumber("110a"));
		TEST_ASSERT(!isNumber("--"));
		TEST_ASSERT(!isNumber("+"));
		TEST_ASSERT(!isNumber("."));
		TEST_ASSERT(!isNumber("a110"));
		TEST_ASSERT(!isNumber("1a10"));
		TEST_ASSERT(!isNumber("110a.0"));
		TEST_ASSERT(!isNumber("1+10.0"));
		TEST_ASSERT(!isNumber("1231-10.0"));
		TEST_ASSERT(!isNumber("*123110.123410"));
		TEST_ASSERT(!isNumber("1.23110.123410"));
		TEST_ASSERT(!isNumber("false"));
		TEST_ASSERT(!isNumber("true"));
		TEST_ASSERT(!isNumber("False"));
		TEST_ASSERT(!isNumber("True"));
	}

	{
		TEST_ASSERT(isInteger(U"10"));
		TEST_ASSERT(isInteger(U"112355"));
		TEST_ASSERT(isInteger(U"776213810"));
		TEST_ASSERT(isInteger(U"+15"));
		TEST_ASSERT(isInteger(U"-10"));
		TEST_ASSERT(!isInteger(U"+15+"));
		TEST_ASSERT(!isInteger(U"-10-"));
		TEST_ASSERT(!isInteger(U"+"));
		TEST_ASSERT(!isInteger(U"-"));
		TEST_ASSERT(!isInteger(U"1-10"));
		TEST_ASSERT(!isInteger(U"1+10"));
		TEST_ASSERT(!isInteger(U"a110"));
		TEST_ASSERT(!isInteger(U"1a10"));
		TEST_ASSERT(!isInteger(U"110a"));
		TEST_ASSERT(!isInteger(U"1.10"));

		TEST_ASSERT(isReal(U"1.0"));
		TEST_ASSERT(isReal(U"21."));
		TEST_ASSERT(isReal(U".13"));
		TEST_ASSERT(isReal(U"123.49"));
		TEST_ASSERT(isReal(U"456.78"));
		TEST_ASSERT(isReal(U"+12456.78"));
		TEST_ASSERT(isReal(U"-12456.78"));
		TEST_ASSERT(isReal(U"123110.123410"));
		TEST_ASSERT(!isReal(U"--"));
		TEST_ASSERT(!isReal(U"+"));
		TEST_ASSERT(!isReal(U"."));
		TEST_ASSERT(!isReal(U"1"));
		TEST_ASSERT(!isReal(U"10"));
		TEST_ASSERT(!isReal(U"a110"));
		TEST_ASSERT(!isReal(U"1a10"));
		TEST_ASSERT(!isReal(U"110a.0"));
		TEST_ASSERT(!isReal(U"1+10.0"));
		TEST_ASSERT(!isReal(U"1231-10.0"));
		TEST_ASSERT(!isReal(U"*123110.123410"));
		TEST_ASSERT(!isReal(U"1.23110.123410"));

		TEST_ASSERT(isBoolean(U"1"));
		TEST_ASSERT(isBoolean(U"0"));
		TEST_ASSERT(isBoolean(U"false"));
		TEST_ASSERT(isBoolean(U"true"));
		TEST_ASSERT(!isBoolean(U"False"));
		TEST_ASSERT(!isBoolean(U"True"));
		TEST_ASSERT(!isBoolean(U"10"));
		TEST_ASSERT(!isBoolean(U"112355"));
		TEST_ASSERT(!isBoolean(U"776213810"));
		TEST_ASSERT(!isBoolean(U"+15"));
		TEST_ASSERT(!isBoolean(U"-10"));
		TEST_ASSERT(!isBoolean(U"+15+"));
		TEST_ASSERT(!isBoolean(U"-10-"));
		TEST_ASSERT(!isBoolean(U"+"));
		TEST_ASSERT(!isBoolean(U"-"));
		TEST_ASSERT(!isBoolean(U"1-10"));
		TEST_ASSERT(!isBoolean(U"1+10"));
		TEST_ASSERT(!isBoolean(U"a110"));
		TEST_ASSERT(!isBoolean(U"1a10"));
		TEST_ASSERT(!isBoolean(U"110a"));
		TEST_ASSERT(!isBoolean(U"1.10"));

		TEST_ASSERT(isNumber(U"10"));
		TEST_ASSERT(isNumber(U"112355"));
		TEST_ASSERT(isNumber(U"776213810"));
		TEST_ASSERT(isNumber(U"+15"));
		TEST_ASSERT(isNumber(U"-10"));
		TEST_ASSERT(isNumber(U"1.0"));
		TEST_ASSERT(isNumber(U"21."));
		TEST_ASSERT(isNumber(U".13"));
		TEST_ASSERT(isNumber(U"123.49"));
		TEST_ASSERT(isNumber(U"456.78"));
		TEST_ASSERT(isNumber(U"+12456.78"));
		TEST_ASSERT(isNumber(U"-12456.78"));
		TEST_ASSERT(isNumber(U"123110.123410"));
		TEST_ASSERT(!isNumber(U"+15+"));
		TEST_ASSERT(!isNumber(U"-10-"));
		TEST_ASSERT(!isNumber(U"+"));
		TEST_ASSERT(!isNumber(U"-"));
		TEST_ASSERT(!isNumber(U"1-10"));
		TEST_ASSERT(!isNumber(U"1+10"));
		TEST_ASSERT(!isNumber(U"a110"));
		TEST_ASSERT(!isNumber(U"1a10"));
		TEST_ASSERT(!isNumber(U"110a"));
		TEST_ASSERT(!isNumber(U"--"));
		TEST_ASSERT(!isNumber(U"+"));
		TEST_ASSERT(!isNumber(U"."));
		TEST_ASSERT(!isNumber(U"a110"));
		TEST_ASSERT(!isNumber(U"1a10"));
		TEST_ASSERT(!isNumber(U"110a.0"));
		TEST_ASSERT(!isNumber(U"1+10.0"));
		TEST_ASSERT(!isNumber(U"1231-10.0"));
		TEST_ASSERT(!isNumber(U"*123110.123410"));
		TEST_ASSERT(!isNumber(U"1.23110.123410"));
		TEST_ASSERT(!isNumber(U"false"));
		TEST_ASSERT(!isNumber(U"true"));
		TEST_ASSERT(!isNumber(U"False"));
		TEST_ASSERT(!isNumber(U"True"));
	}
}
TEST_END(TextNumberChecks)



TEST_BEGIN(TextEquals)
{
	{
		TEST_ASSERT(equals("super mario world", "super mario world"));
		TEST_ASSERT(!equals("super mario world", "super Mario world"));
		TEST_ASSERT(!equals("super mario", "super Mario world"));

		TEST_ASSERT(equals("super mario world", "super MARIO world", false));
	}

	{
		TEST_ASSERT(equals(U"super mario world", U"super mario world"));
		TEST_ASSERT(!equals(U"super mario world", U"super Mario world"));
		TEST_ASSERT(!equals(U"super mario", U"super Mario world"));

		TEST_ASSERT(equals(U"super mario world", U"super MARIO world", false));
	}
}
TEST_END(TextEquals)


TEST_BEGIN(TextJoin)
{
	{
		TEST_FAIL_NOT_EQUAL("1|2|3", join(std::vector<int>{1, 2, 3}, "|"));
		TEST_FAIL_NOT_EQUAL("1AA2AA3", join(std::vector<int>{1, 2, 3}, "AA"));

		TEST_FAIL_NOT_EQUAL("4.55, 3, 7.4", join(std::vector{4.55, 3.0, 7.4}, ", "));

		TEST_FAIL_NOT_EQUAL("1.2", join(std::vector{ 1.2f }, ", "));
	}

	
	str32 d = join(std::vector<int>{1, 2, 3}, U"|");
	{

		TEST_FAIL_NOT_EQUAL(U"1|2|3", join(std::vector<int>{1, 2, 3}, U"|"));
		TEST_FAIL_NOT_EQUAL(U"1AA2AA3", join(std::vector<int>{1, 2, 3}, U"AA"));

		TEST_FAIL_NOT_EQUAL(U"4.55, 3, 7.4", join(std::vector{ 4.55, 3.0, 7.4 }, U", "));

		TEST_FAIL_NOT_EQUAL(U"1.2", join(std::vector{ 1.2f }, U", "));
	}
}
TEST_END(TextJoin)


TEST_BEGIN(TextUnicode)
{
	{
		std::u8string sample8 = u8"‘single’ and “double” quotes";
		str32 sample32 = U"‘single’ and “double” quotes";
		TEST_FAIL_NOT_EQUAL(28, (int)sample32.size());
		TEST_FAIL_NOT_EQUAL(sample32, str8To32(u8To8(sample8)));
	}

	{
		int i = 0;

		str32 s32 = U"κόσμε";
		i = 0;
		TEST_FAIL_NOT_EQUAL(5, (int)s32.size());
		TEST_FAIL_NOT_EQUAL(0x000003BA, s32[i++]); //GREEK SMALL LETTER KAPPA
		TEST_FAIL_NOT_EQUAL(0x00001F79, s32[i++]); //GREEK SMALL LETTER OMICRON WITH OXIA
		TEST_FAIL_NOT_EQUAL(0x000003C3, s32[i++]); //GREEK SMALL LETTER SIGMA
		TEST_FAIL_NOT_EQUAL(0x000003BC, s32[i++]); //GREEK SMALL LETTER MU
		TEST_FAIL_NOT_EQUAL(0x000003B5, s32[i++]); //GREEK SMALL LETTER EPSILON

		str8 s8 = u8To8(u8"κόσμε");
		i = 0;
		TEST_FAIL_NOT_EQUAL(11, (int)s8.size());
		TEST_FAIL_NOT_EQUAL(0xCE, (uint8_t)s8[i++]);
		TEST_FAIL_NOT_EQUAL(0xBA, (uint8_t)s8[i++]);
		TEST_FAIL_NOT_EQUAL(0xE1, (uint8_t)s8[i++]);
		TEST_FAIL_NOT_EQUAL(0xBD, (uint8_t)s8[i++]);
		TEST_FAIL_NOT_EQUAL(0xB9, (uint8_t)s8[i++]);
		TEST_FAIL_NOT_EQUAL(0xCF, (uint8_t)s8[i++]);
		TEST_FAIL_NOT_EQUAL(0x83, (uint8_t)s8[i++]);
		TEST_FAIL_NOT_EQUAL(0xCE, (uint8_t)s8[i++]);
		TEST_FAIL_NOT_EQUAL(0xBC, (uint8_t)s8[i++]);
		TEST_FAIL_NOT_EQUAL(0xCE, (uint8_t)s8[i++]);
		TEST_FAIL_NOT_EQUAL(0xB5, (uint8_t)s8[i++]);

		TEST_FAIL_NOT_EQUAL(s32, str8To32(s8));
		TEST_FAIL_NOT_EQUAL(s8, str32To8(str8To32(s8)));
		TEST_FAIL_NOT_EQUAL(s8, str32To8(s32));
	}

	{
		str16 s16 = u"👩 ABCDEFG 🤦";

		int i = 0;
		TEST_FAIL_NOT_EQUAL(13, (int)s16.size());
		TEST_FAIL_NOT_EQUAL(0xD83D, (int32_t)s16[i++]);
		TEST_FAIL_NOT_EQUAL(0xDC69, (int32_t)s16[i++]);
		TEST_FAIL_NOT_EQUAL(0x0020, (int32_t)s16[i++]);
		TEST_FAIL_NOT_EQUAL(0x0041, (int32_t)s16[i++]);
		TEST_FAIL_NOT_EQUAL(0x0042, (int32_t)s16[i++]);
		TEST_FAIL_NOT_EQUAL(0x0043, (int32_t)s16[i++]);
		TEST_FAIL_NOT_EQUAL(0x0044, (int32_t)s16[i++]);
		TEST_FAIL_NOT_EQUAL(0x0045, (int32_t)s16[i++]);
		TEST_FAIL_NOT_EQUAL(0x0046, (int32_t)s16[i++]);
		TEST_FAIL_NOT_EQUAL(0x0047, (int32_t)s16[i++]);
		TEST_FAIL_NOT_EQUAL(0x0020, (int32_t)s16[i++]);
		TEST_FAIL_NOT_EQUAL(0xD83E, (int32_t)s16[i++]);
		TEST_FAIL_NOT_EQUAL(0xDD26, (int32_t)s16[i++]);

		str32 s32 = str16To32(s16);

		//std::cout << "16>>" << debugStr(s16) << std::endl;
		//std::cout << "32>>" << debugStr(s32) << std::endl;

		i = 0;
		TEST_FAIL_NOT_EQUAL(11, (int)s32.size());
		TEST_FAIL_NOT_EQUAL(0x1F469, (int32_t)s32[i++]);
		TEST_FAIL_NOT_EQUAL(0x0020, (int32_t)s32[i++]);
		TEST_FAIL_NOT_EQUAL(0x0041, (int32_t)s32[i++]);
		TEST_FAIL_NOT_EQUAL(0x0042, (int32_t)s32[i++]);
		TEST_FAIL_NOT_EQUAL(0x0043, (int32_t)s32[i++]);
		TEST_FAIL_NOT_EQUAL(0x0044, (int32_t)s32[i++]);
		TEST_FAIL_NOT_EQUAL(0x0045, (int32_t)s32[i++]);
		TEST_FAIL_NOT_EQUAL(0x0046, (int32_t)s32[i++]);
		TEST_FAIL_NOT_EQUAL(0x0047, (int32_t)s32[i++]);
		TEST_FAIL_NOT_EQUAL(0x0020, (int32_t)s32[i++]);
		TEST_FAIL_NOT_EQUAL(0x1F926, (int32_t)s32[i++]);
	}
    
    
    
    {
        str32 s32_source = U"emojis_😍😒😖😟.txt";
        str32 s32 = str8To32(str32To8(U"emojis_😍😒😖😟.txt"));
        TEST_FAIL_NOT_EQUAL(s32_source, s32);
        
        TEST_FAIL_NOT_EQUAL(15, (int)s32.size());
        
        int i = 0;
        TEST_FAIL_NOT_EQUAL('e', s32_source[i++]);
        TEST_FAIL_NOT_EQUAL('m', s32_source[i++]);
        TEST_FAIL_NOT_EQUAL('o', s32_source[i++]);
        TEST_FAIL_NOT_EQUAL('j', s32_source[i++]);
        TEST_FAIL_NOT_EQUAL('i', s32_source[i++]);
        TEST_FAIL_NOT_EQUAL('s', s32_source[i++]);
        TEST_FAIL_NOT_EQUAL('_', s32_source[i++]);
        TEST_FAIL_NOT_EQUAL(0x1F60D, s32_source[i++]);
        TEST_FAIL_NOT_EQUAL(0x1F612, s32_source[i++]);
        TEST_FAIL_NOT_EQUAL(0x1F616, s32_source[i++]);
        TEST_FAIL_NOT_EQUAL(0x1F61F, s32_source[i++]);
        TEST_FAIL_NOT_EQUAL('.', s32_source[i++]);
        TEST_FAIL_NOT_EQUAL('t', s32_source[i++]);
        TEST_FAIL_NOT_EQUAL('x', s32_source[i++]);
        TEST_FAIL_NOT_EQUAL('t', s32_source[i++]);
    }
    

	{
		str16 s16 = u"👩 ABCDEFG 🤦";
		str32 s32 = str16To32(s16);
		str16 s16_copy = str32To16(s32);

		int i = 0;
		TEST_FAIL_NOT_EQUAL(13, (int)s16_copy.size());
		TEST_FAIL_NOT_EQUAL(0xD83D, (int32_t)s16_copy[i++]);
		TEST_FAIL_NOT_EQUAL(0xDC69, (int32_t)s16_copy[i++]);
		TEST_FAIL_NOT_EQUAL(0x0020, (int32_t)s16_copy[i++]);
		TEST_FAIL_NOT_EQUAL(0x0041, (int32_t)s16_copy[i++]);
		TEST_FAIL_NOT_EQUAL(0x0042, (int32_t)s16_copy[i++]);
		TEST_FAIL_NOT_EQUAL(0x0043, (int32_t)s16_copy[i++]);
		TEST_FAIL_NOT_EQUAL(0x0044, (int32_t)s16_copy[i++]);
		TEST_FAIL_NOT_EQUAL(0x0045, (int32_t)s16_copy[i++]);
		TEST_FAIL_NOT_EQUAL(0x0046, (int32_t)s16_copy[i++]);
		TEST_FAIL_NOT_EQUAL(0x0047, (int32_t)s16_copy[i++]);
		TEST_FAIL_NOT_EQUAL(0x0020, (int32_t)s16_copy[i++]);
		TEST_FAIL_NOT_EQUAL(0xD83E, (int32_t)s16_copy[i++]);
		TEST_FAIL_NOT_EQUAL(0xDD26, (int32_t)s16_copy[i++]);
	}
}
TEST_END(TextUnicode)


TEST_BEGIN(TextGlob)
{
	{
		TEST_ASSERT(glob("main.?", "main.c"));
		TEST_ASSERT(!glob("index.?", "main.c"));
	
		TEST_ASSERT(glob("?at", "Cat"));
		TEST_ASSERT(glob("?at", "cat"));
		TEST_ASSERT(glob("?at", "Bat"));
		TEST_ASSERT(glob("?at", "bat"));
		TEST_ASSERT(!glob("?at", "at"));

		TEST_ASSERT(glob("*", "main.c"));
        TEST_ASSERT(glob("*.c", "main.c"));
		TEST_ASSERT(glob("*.c", "index.c"));
		TEST_ASSERT(glob("*.c", "test.c"));
		TEST_ASSERT(!glob("*.js", "main.c"));

		TEST_ASSERT(glob("Law*", "Law"));
		TEST_ASSERT(glob("Law*", "Laws"));
		TEST_ASSERT(glob("Law*", "Lawyer"));
		TEST_ASSERT(!glob("Law*", "GrokLaw"));
		TEST_ASSERT(!glob("Law*", "La"));
        TEST_ASSERT(!glob("Law*", "aw"));
      
		TEST_ASSERT(glob("*Law*", "Law"));
		TEST_ASSERT(glob("*Law*", "GrokLaw"));
		TEST_ASSERT(glob("*Law*", "Lawyer"));
		TEST_ASSERT(!glob("*Law*", "La"));
		TEST_ASSERT(!glob("*Law*", "aw"));

		TEST_ASSERT(glob("*.[abc]", "main.a"));
		TEST_ASSERT(glob("*.[abc]", "main.b"));
		TEST_ASSERT(glob("*.[abc]", "main.c"));
		TEST_ASSERT(!glob("*.[abc]", "main.d"));
		TEST_ASSERT(!glob("*.[abc", "main.a"));
     
		TEST_ASSERT(glob("[CB]at", "Cat"));
		TEST_ASSERT(glob("[CB]at", "Bat"));
		TEST_ASSERT(!glob("[CB]at", "cat"));
		TEST_ASSERT(!glob("[CB]at", "bat"));
		TEST_ASSERT(!glob("[CB]at", "CBat"));

		TEST_ASSERT(glob("[][!]", "]"));
		TEST_ASSERT(glob("[][!]", "["));
		TEST_ASSERT(glob("[][!]", "!"));
		TEST_ASSERT(!glob("[][!]", "a"));
		TEST_ASSERT(!glob("[][!", "]"));

		TEST_ASSERT(glob("Letter[0-9]", "Letter0"));
		TEST_ASSERT(glob("Letter[0-9]", "Letter1"));
		TEST_ASSERT(glob("Letter[0-9]", "Letter2"));
		TEST_ASSERT(glob("Letter[0-9]", "Letter9"));

		TEST_ASSERT(!glob("Letter[0-9]", "Letters"));
		TEST_ASSERT(!glob("Letter[0-9]", "Letter"));
		TEST_ASSERT(!glob("Letter[0-9]", "Letter10"));

		TEST_ASSERT(glob("[A-Fa-f0-9]", "A"));
		TEST_ASSERT(glob("[A-Fa-f0-9]", "a"));
		TEST_ASSERT(glob("[A-Fa-f0-9]", "B"));
		TEST_ASSERT(glob("[A-Fa-f0-9]", "b"));
		TEST_ASSERT(glob("[A-Fa-f0-9]", "0"));
		TEST_ASSERT(!glob("[A-Fa-f0-9]", "-"));

		TEST_ASSERT(glob("[]-]", "]"));
		
		TEST_ASSERT(glob("[]-]", "-"));
		TEST_ASSERT(!glob("[]-]", "a"));

		TEST_ASSERT(glob("[--0]", "-"));
		TEST_ASSERT(glob("[--0]", "."));
		TEST_ASSERT(glob("[--0]", "0"));
		TEST_ASSERT(glob("[--0]", "/"));
		TEST_ASSERT(!glob("[--0]", "a"));
  
		TEST_ASSERT(glob("[!]a-]", "b"));
		TEST_ASSERT(!glob("[!]a-]", "]"));
		TEST_ASSERT(!glob("[!]a-]", "a"));
		TEST_ASSERT(!glob("[!]a-]", "-"));
	 
		TEST_ASSERT(glob("[[?*\\]", "["));
		TEST_ASSERT(glob("[[?*\\]", "?"));
		TEST_ASSERT(glob("[[?*\\]", "*"));
		TEST_ASSERT(glob("[[?*\\]", "\\"));
		TEST_ASSERT(!glob("[[?*\\]", "a"));

		TEST_ASSERT(glob("\\*", "*"));

		TEST_ASSERT(glob(U"[Пп]ривет, [Мм]ир", U"Привет, Мир"));

		TEST_ASSERT(!glob("[", "aaaaa"));
		TEST_ASSERT(!glob("[!", "aaaaa"));
		TEST_ASSERT(!glob("[--", "aaaaa"));
		TEST_ASSERT(!glob("\\", "aaaaa"));
	}

	{
		str8 e0 = "super mario";
		str8 e1 = "super merio world";
		str8 e2 = "super mirio bros";
		str8 e3 = "super mario kart";
		str8 e4 = "super luigi";
		auto result = glob("super m?rio", { e0, e1, e2, e3, e4 });
		TEST_FAIL_NOT_EQUAL(result.size(), size_t(1));
	}
    
    {
        str8 e0 = "super mario";
        str8 e1 = "super merio";
        str8 e2 = "super mirio";
        str8 e3 = "super mario";
        str8 e4 = "super luigi";
        auto result = glob("super m?rio", { e0, e1, e2, e3, e4 });
        TEST_FAIL_NOT_EQUAL(result.size(), size_t(4));
        TEST_FAIL_NOT_EQUAL(result[0], e0);
        TEST_FAIL_NOT_EQUAL(result[1], e1);
        TEST_FAIL_NOT_EQUAL(result[2], e2);
        TEST_FAIL_NOT_EQUAL(result[3], e3);
    }
}
TEST_END(TextGlob)


TEST_BEGIN(TextLevenshteinDistance)
{
    {
        TEST_FAIL_NOT_EQUAL(size_t(0), levenshteinDistance("hello", "hello"));
        TEST_FAIL_NOT_EQUAL(size_t(3), levenshteinDistance("hitting", "kitten"));
        TEST_FAIL_NOT_EQUAL(size_t(2), levenshteinDistance("book", "back"));
        TEST_FAIL_NOT_EQUAL(size_t(5), levenshteinDistance("super mario", "super luigi"));
        TEST_FAIL_NOT_EQUAL(size_t(12), levenshteinDistance("hello dramatic world", "a brave new world"));
        TEST_FAIL_NOT_EQUAL(size_t(13), levenshteinDistance("hello dramatic world1", "a brave new world2"));
        TEST_FAIL_NOT_EQUAL(size_t(12), levenshteinDistance("hello dramatic world1", "a brave new world1"));
        TEST_FAIL_NOT_EQUAL(size_t(18), levenshteinDistance("", "a brave new world1"));
    }
    
    
    {
        str8 a0 = "super mario";
        str8 a1 = "super merio";
        str8 a2 = "super mirio";
        str8 a3 = "super luigi";
        str8 a4 = "super sdd ajjd gjjg ajja djg";
        std::vector<str8> results = similiarSort(a0, { a1, a4, a2, a3, a0 });
        TEST_FAIL_NOT_EQUAL(size_t(5), results.size());
        TEST_FAIL_NOT_EQUAL(a0, results[0]);
        TEST_FAIL_NOT_EQUAL(a4, results[4]);

    }
}
TEST_END(TextLevenshteinDistance)
