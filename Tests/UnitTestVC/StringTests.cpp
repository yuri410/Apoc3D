#include "TestCommon.h"
#include "Apoc3D/ApocString.h"

namespace UnitTestVC
{
	TEST_CLASS(StringUtilsTest)
	{
	public:
		TEST_METHOD(StringUtils_ParseBool)
		{
			bool val = StringUtils::ParseBool("true");
			Assert::IsTrue(val);
			val = StringUtils::ParseBool("1");
			Assert::IsTrue(val);
			val = StringUtils::ParseBool("yes");
			Assert::IsTrue(val);
			val = StringUtils::ParseBool("true...");
			Assert::IsTrue(val);

			val = StringUtils::ParseBool("false");
			Assert::IsFalse(val);
			val = StringUtils::ParseBool("dsad");
			Assert::IsFalse(val);
			val = StringUtils::ParseBool("no");
			Assert::IsFalse(val);
			val = StringUtils::ParseBool("0");
			Assert::IsFalse(val);

			val = StringUtils::ParseBool(L"true");
			Assert::IsTrue(val);
			val = StringUtils::ParseBool(L"1");
			Assert::IsTrue(val);
			val = StringUtils::ParseBool(L"yes");
			Assert::IsTrue(val);
			val = StringUtils::ParseBool(L"true...");
			Assert::IsTrue(val);

			val = StringUtils::ParseBool(L"false");
			Assert::IsFalse(val);
			val = StringUtils::ParseBool(L"dsad");
			Assert::IsFalse(val);
			val = StringUtils::ParseBool(L"no");
			Assert::IsFalse(val);
			val = StringUtils::ParseBool(L"0");
			Assert::IsFalse(val);
		}

		TEST_METHOD(StringUtils_ParseInt)
		{
			int32 v = StringUtils::ParseInt32(L"10");
			Assert::AreEqual(10, v);

			v = StringUtils::ParseInt32(L"+11");
			Assert::AreEqual(11, v);

			v = StringUtils::ParseInt32(L"+0");
			Assert::AreEqual(0, v);

			v = StringUtils::ParseInt32(L"-9");
			Assert::AreEqual(-9, v);

			v = StringUtils::ParseInt32(L"-0");
			Assert::AreEqual(-0, v);

			v = StringUtils::ParseInt32(L"2147483647");
			Assert::AreEqual(2147483647, v);

			v = StringUtils::ParseInt32(L"-2147483648");
			Assert::AreEqual((int32)0x80000000, v);
		}

		TEST_METHOD(StringUtils_IntToString)
		{
			String v = StringUtils::IntToString(10);
			Assert::AreEqual(String(L"10"), v);

			v = StringUtils::IntToString(11, StringUtils::SF_ShowPositiveSign);
			Assert::AreEqual(String(L"+11"), v);

			v = StringUtils::IntToString(0, StringUtils::SF_ShowPositiveSign);
			Assert::AreEqual(String(L"0"), v);

			v = StringUtils::IntToString(-93);
			Assert::AreEqual(String(L"-93"), v);

			v = StringUtils::IntToString(2147483647);
			Assert::AreEqual(String(L"2147483647"), v);

			v = StringUtils::IntToString((int32)0x80000000);
			Assert::AreEqual(String(L"-2147483648"), v);

			//////////////////////////////////////////////////////////////////////////

			v = StringUtils::IntToString(11, StrFmt::a<6>::val | StringUtils::SF_ShowPositiveSign | StringUtils::SF_Left);
			Assert::AreEqual(String(L"+11   "), v);

			v = StringUtils::IntToString(0, StrFmt::a<6, '0'>::val | StringUtils::SF_ShowPositiveSign | StringUtils::SF_Left);
			Assert::AreEqual(String(L"000000"), v);

			v = StringUtils::IntToString(-951, StrFmt::a<6>::val);
			Assert::AreEqual(String(L"  -951"), v);

			v = StringUtils::IntToString(-951, StrFmt::a<6, '0'>::val);
			Assert::AreEqual(String(L"-00951"), v);

		}

		TEST_METHOD(StringUtils_ParseUInt)
		{
			uint32 v = StringUtils::ParseUInt32(L"10");
			Assert::AreEqual(10U, v);

			v = StringUtils::ParseUInt32(L"+11");
			Assert::AreEqual(11U, v);

			v = StringUtils::ParseUInt32(L"+000");
			Assert::AreEqual(0U, v);

			v = StringUtils::ParseUInt32(L"4294967295");
			Assert::AreEqual(4294967295U, v);

			uint64 vl = StringUtils::ParseUInt64(L"1152921504606846975");
			Assert::AreEqual(1152921504606846975UL, vl);

			v = StringUtils::ParseUInt32Hex(L"0xdEAdbEEf");
			Assert::AreEqual(0xdEAdbEEfU, v);

			vl = StringUtils::ParseUInt64Hex(L"FFFFFFFFFFFFFFF");
			Assert::AreEqual(0xFFFFFFFFFFFFFFFUL, vl);

			vl = StringUtils::ParseUInt64Bin(L"111111111111111111111111111111111111111111111111111111111111");
			Assert::AreEqual(0xFFFFFFFFFFFFFFFUL, vl);

			v = StringUtils::ParseUInt32Bin(L"11011110101011011011111011101111");
			Assert::AreEqual(0xdEAdbEEfU, v);

		}

		TEST_METHOD(StringUtils_UIntToString)
		{
			String v = StringUtils::UIntToString(10U);
			Assert::AreEqual(String(L"10"), v);

			v = StringUtils::UIntToString(11U, StringUtils::SF_ShowPositiveSign);
			Assert::AreEqual(String(L"+11"), v);

			v = StringUtils::UIntToString(0U, StringUtils::SF_ShowPositiveSign);
			Assert::AreEqual(String(L"0"), v);

			v = StringUtils::UIntToString(0U, StrFmt::a<3, '0'>::val | StringUtils::SF_ShowPositiveSign);
			Assert::AreEqual(String(L"000"), v);

			v = StringUtils::UIntToString(4294967295U);
			Assert::AreEqual(String(L"4294967295"), v);

			v = StringUtils::UIntToString(1152921504606846975UL);
			Assert::AreEqual(String(L"1152921504606846975"), v);

			v = StringUtils::UIntToStringHex(0xdEAdbEEfU, StringUtils::SF_ShowHexBase | StringUtils::SF_UpperCase);
			Assert::AreEqual(String(L"0xDEADBEEF"), v);

			v = StringUtils::UIntToStringHex(0xdEAdbEEfU);
			Assert::AreEqual(String(L"deadbeef"), v);

			v = StringUtils::UIntToStringBin(0xFFFFFFFFFFFFFFFUL);
			Assert::AreEqual(String(L"111111111111111111111111111111111111111111111111111111111111"), v);

			v = StringUtils::UIntToStringBin(0xdEAdbEEfU);
			Assert::AreEqual(String(L"11011110101011011011111011101111"), v);

			//////////////////////////////////////////////////////////////////////////

			v = StringUtils::UIntToString(11U, StrFmt::a<6>::val | StringUtils::SF_ShowPositiveSign | StringUtils::SF_Left);
			Assert::AreEqual(String(L"+11   "), v);

			v = StringUtils::UIntToString(0U, StrFmt::a<6, '0'>::val | StringUtils::SF_ShowPositiveSign | StringUtils::SF_Left);
			Assert::AreEqual(String(L"000000"), v);

			v = StringUtils::UIntToStringHex(0xffU, StrFmt::a<6>::val | StringUtils::SF_ShowHexBase);
			Assert::AreEqual(String(L"  0xff"), v);

			v = StringUtils::UIntToStringHex(0xffU, StrFmt::a<6, '0'>::val | StringUtils::SF_ShowHexBase);
			Assert::AreEqual(String(L"0x00ff"), v);

			v = StringUtils::UIntToStringHex(0x1fU, StrFmt::a<6, '0'>::val);
			Assert::AreEqual(String(L"00001f"), v);

			v = StringUtils::UIntToStringBin(0xfU, StrFmt::a<6, '0'>::val);
			Assert::AreEqual(String(L"001111"), v);

		}


		TEST_METHOD(StringUtils_ParseDouble)
		{
			double v = StringUtils::ParseDouble(L"1.2345");
			Assert::AreEqual(1.2345, v);

			v = StringUtils::ParseDouble(L"0.12345");
			Assert::AreEqual(0.12345, v);

			v = StringUtils::ParseDouble(L"123.45");
			Assert::AreEqual(123.45, v);

			v = StringUtils::ParseDouble(L"1.234e+5");
			Assert::AreEqual(1.234e+5, v);

			v = StringUtils::ParseDouble(L"1.234e+10");
			Assert::AreEqual(1.234e+10, v);

			v = StringUtils::ParseDouble(L"12.34e+10");
			Assert::AreEqual(12.34e+10, v);

			//////////////////////////////////////////////////////////////////////////

			v = StringUtils::ParseDouble(L"+1.2345");
			Assert::AreEqual(1.2345, v);

			v = StringUtils::ParseDouble(L"+0.12345");
			Assert::AreEqual(0.12345, v);

			v = StringUtils::ParseDouble(L"+123.45");
			Assert::AreEqual(123.45, v);

			v = StringUtils::ParseDouble(L"+1.234e+5");
			Assert::AreEqual(1.234e+5, v);

			v = StringUtils::ParseDouble(L"+1.234e+10");
			Assert::AreEqual(1.234e+10, v);

			v = StringUtils::ParseDouble(L"+12.34e+10");
			Assert::AreEqual(12.34e+10, v);

			//////////////////////////////////////////////////////////////////////////

			v = StringUtils::ParseDouble(L"-1.2345");
			Assert::AreEqual(-1.2345, v);

			v = StringUtils::ParseDouble(L"-0.12345");
			Assert::AreEqual(-0.12345, v);

			v = StringUtils::ParseDouble(L"-123.45");
			Assert::AreEqual(-123.45, v);

			v = StringUtils::ParseDouble(L"-1.234e+5");
			Assert::AreEqual(-1.234e+5, v);

			v = StringUtils::ParseDouble(L"-1.234e+10");
			Assert::AreEqual(-1.234e+10, v);

			v = StringUtils::ParseDouble(L"-12.34e+10");
			Assert::AreEqual(-12.34e+10, v);

			//////////////////////////////////////////////////////////////////////////

			v = StringUtils::ParseDouble(L"-1.234e-5");
			Assert::AreEqual(-1.234e-5, v);

			v = StringUtils::ParseDouble(L"+1.234e-10");
			Assert::AreEqual(1.234e-10, v);

			v = StringUtils::ParseDouble(L"-12.34e-10");
			Assert::AreEqual(-12.34e-10, v);
		}

		TEST_METHOD(StringUtils_DoubleToString)
		{
			String v = StringUtils::DoubleToString(1.2345, StrFmt::fpdec<5>::val);
			Assert::AreEqual(String(L"1.23450"), v);

			v = StringUtils::DoubleToString(+1.234e-10, StrFmt::fp<3>::val | StringUtils::SF_FPScientific);
			Assert::AreEqual(String(L"1.234e-10"), v);

			v = StringUtils::DoubleToString(+1.234e-10, StrFmt::fpa<12, '0', 3>::val | StringUtils::SF_FPScientific | StringUtils::SF_ShowPositiveSign | StringUtils::SF_UpperCase);
			Assert::AreEqual(String(L"+001.234E-10"), v);

			v = StringUtils::DoubleToString(-3.540e-9, StrFmt::fp<3>::val | StringUtils::SF_FPScientific);
			Assert::AreEqual(String(L"-3.540e-9"), v);

			v = StringUtils::DoubleToString(-3.5408e-9, StrFmt::fp<3>::val | StringUtils::SF_FPScientific);
			Assert::AreEqual(String(L"-3.541e-9"), v);




			v = StringUtils::DoubleToString(10.23, StringUtils::SF_FPDecimal);
			Assert::AreEqual(String(L"10.23"), v);

			v = StringUtils::DoubleToString(00.123, StrFmt::fp<4>::val);
			Assert::AreEqual(String(L"0.1230"), v);

			v = StringUtils::DoubleToString(0.001, StringUtils::SF_FPDecimal);
			Assert::AreEqual(String(L"0.001"), v);

			v = StringUtils::DoubleToString(0.001, StrFmt::fp<4>::val);
			Assert::AreEqual(String(L"0.0010"), v);

			v = StringUtils::DoubleToString(0.0, StrFmt::fp<4>::val);
			Assert::AreEqual(String(L"0.0000"), v);

			v = StringUtils::DoubleToString(-0.0, StrFmt::fp<3>::val);
			Assert::AreEqual(String(L"-0.000"), v);

			v = StringUtils::DoubleToString(-0.0001, StrFmt::fp<4>::val);
			Assert::AreEqual(String(L"-0.0001"), v);

			
			v = StringUtils::DoubleToString(123456789e+128, StringUtils::SF_FPScientific);
			Assert::AreEqual(String(L"1.23456789e+136"), v);

			v = StringUtils::DoubleToString(123456789e+128, StringUtils::SF_FPDecimal);
			Assert::AreEqual(String(L"12345678900000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"), v);


			v = StringUtils::DoubleToString(54321.84964, StringUtils::SF_FPDecimal);
			Assert::AreEqual(String(L"54321.84964"), v);


			v = StringUtils::DoubleToString(98.7654327, StringUtils::SF_FPDecimal);
			Assert::AreEqual(String(L"98.7654327"), v);

			v = StringUtils::DoubleToString(98.127, StrFmt::fp<0>::val | StringUtils::SF_FPDecimal);
			Assert::AreEqual(String(L"98"), v);

			v = StringUtils::DoubleToString(98.7654327, StrFmt::fp<0>::val | StringUtils::SF_FPDecimal);
			Assert::AreEqual(String(L"99"), v);



			v = StringUtils::DoubleToString(45645.003400, StringUtils::SF_FPDecimal);
			Assert::AreEqual(String(L"45645.0034"), v);

			v = StringUtils::DoubleToString(46589.000, StringUtils::SF_FPDecimal);
			Assert::AreEqual(String(L"46589"), v);



			v = StringUtils::DoubleToString(INFINITY);
			Assert::AreEqual(String(L"inf"), v);

			v = StringUtils::DoubleToString(INFINITY, StringUtils::SF_ShowPositiveSign | StringUtils::SF_UpperCase);
			Assert::AreEqual(String(L"+INF"), v);

			v = StringUtils::DoubleToString(-(double)INFINITY);
			Assert::AreEqual(String(L"-inf"), v);

			v = StringUtils::DoubleToString(NAN);
			Assert::AreEqual(String(L"nan"), v);

			v = StringUtils::DoubleToString(99.8374939, StrFmt::fp<0>::val);
			Assert::AreEqual(String(L"100"), v);

			v = StringUtils::SingleToString(100.0f, StrFmt::fp<0>::val);
			Assert::AreEqual(String(L"100"), v);

		}


		TEST_METHOD(StringUtils_StartEnd)
		{
			bool r = StringUtils::StartsWith(L"sd", L"", false);
			Assert::IsFalse(r);
			r = StringUtils::StartsWith(L"dsadfds", L"", false);
			Assert::IsFalse(r);

			r = StringUtils::StartsWith(L"12345", L"123", false);
			Assert::IsTrue(r);
			r = StringUtils::StartsWith(L"abcdef", L"ABC", true);
			Assert::IsTrue(r);

			r = StringUtils::EndsWith(L"abcdef", L"DEF", true);
			Assert::IsTrue(r);
			r = StringUtils::EndsWith(L"12345", L"45", false);
			Assert::IsTrue(r);

		}

		TEST_METHOD(StringUtils_Case)
		{
			String v = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

			StringUtils::ToLowerCase(v);
			Assert::AreEqual(String(L"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz0123456789+/"), v);
			
			v = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

			StringUtils::ToUpperCase(v);
			Assert::AreEqual(String(L"ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+/"), v);
		}

		TEST_METHOD(StringUtils_EqualsNoCase)
		{
			Assert::IsTrue(StringUtils::EqualsNoCase("Aaa", "aAa"));
			Assert::IsFalse(StringUtils::EqualsNoCase("Aab", "aAa"));

			Assert::IsTrue(StringUtils::EqualsNoCase(L"Aaa", L"aAa"));
			Assert::IsFalse(StringUtils::EqualsNoCase(L"Aab", L"aAa"));
		}

		TEST_METHOD(StringUtils_Split)
		{
			String test = L"s , sad,a";
			List<String> results;
			StringUtils::Split(test, results, L", ");

			Assert::AreEqual(results.getCount(), 3);
			Assert::AreEqual(L"s", results[0].c_str());
			Assert::AreEqual(L"sad", results[1].c_str());
			Assert::AreEqual(L"a", results[2].c_str());

			test = L",";
			results.Clear();
			StringUtils::Split(test, results, L", ");
			Assert::AreEqual(0, results.getCount());

			test = L"";
			results.Clear();
			StringUtils::Split(test, results, L", ");
			Assert::AreEqual(0, results.getCount());
		}

		TEST_METHOD(StringUtils_SplitParseInts)
		{
			String test = L"1 , 4,2, 90 ";
			List<int32> results;
			StringUtils::SplitParseInts(test, results, L", ");

			Assert::AreEqual(results.getCount(), 4);
			Assert::AreEqual(1, results[0]);
			Assert::AreEqual(4, results[1]);
			Assert::AreEqual(2, results[2]);
			Assert::AreEqual(90, results[3]);

			test = L"1 , 2, 90 ";
			int32 resultArr[4];
			int32 c = StringUtils::SplitParseInts(test, resultArr, countof(resultArr), L", ");
			Assert::AreEqual(3, c);
			Assert::AreEqual(1, resultArr[0]);
			Assert::AreEqual(2, resultArr[1]);
			Assert::AreEqual(90, resultArr[2]);
		}

		TEST_METHOD(StringUtils_SplitParseSingles)
		{
			String test = L"1.0 , 4,2.0, 90 ";
			List<float> results;
			StringUtils::SplitParseSingles(test, results, L", ");

			Assert::AreEqual(results.getCount(), 4);
			Assert::AreEqual(1.0f, results[0], 0.0001f);
			Assert::AreEqual(4.0f, results[1], 0.0001f);
			Assert::AreEqual(2.0f, results[2], 0.0001f);
			Assert::AreEqual(90.0f, results[3], 0.0001f);

			test = L"1 , 2, 90 ";
			float resultArr[4];
			int32 c = StringUtils::SplitParseSingles(test, resultArr, countof(resultArr), L", ");
			Assert::AreEqual(3, c);
			Assert::AreEqual(1.0f, resultArr[0], 0.0001f);
			Assert::AreEqual(2.0f, resultArr[1], 0.0001f);
			Assert::AreEqual(90.0f, resultArr[2], 0.0001f);
		}
	};

	TEST_CLASS(StringTest)
	{
		MemoryService mem;

	public:
		using Str = StringBase<char>;

	private:

	};
}