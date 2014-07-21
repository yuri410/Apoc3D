
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Apoc3D::Utility;

namespace UnitTestVC
{
	TEST_CLASS(StringUtilsTest)
	{
	public:

		TEST_METHOD(StringParseBool)
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

		TEST_METHOD(StringEqualsNoCase)
		{
			Assert::IsTrue(StringUtils::EqualsNoCase("Aaa", "aAa"));
			Assert::IsFalse(StringUtils::EqualsNoCase("Aab", "aAa"));

			Assert::IsTrue(StringUtils::EqualsNoCase(L"Aaa", L"aAa"));
			Assert::IsFalse(StringUtils::EqualsNoCase(L"Aab", L"aAa"));
		}

		TEST_METHOD(StringSplit)
		{
			String test = L"s , sad,a";
			List<String> results;
			StringUtils::Split(test, results, L", ");

			Assert::AreEqual(results.getCount(), 3);
			Assert::AreEqual(L"s", results[0].c_str());
			Assert::AreEqual(L"sad", results[1].c_str());
			Assert::AreEqual(L"a", results[2].c_str());
		}

		TEST_METHOD(StringSplitParseInts)
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
			int32 c = StringUtils::SplitParseInts(test, resultArr, ARRAYSIZE(resultArr), L", ");
			Assert::AreEqual(3, c);
			Assert::AreEqual(1, resultArr[0]);
			Assert::AreEqual(2, resultArr[1]);
			Assert::AreEqual(90, resultArr[2]);
		}

		TEST_METHOD(StringSplitParseSingles)
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
			int32 c = StringUtils::SplitParseSingles(test, resultArr, ARRAYSIZE(resultArr), L", ");
			Assert::AreEqual(3, c);
			Assert::AreEqual(1.0f, resultArr[0], 0.0001f);
			Assert::AreEqual(2.0f, resultArr[1], 0.0001f);
			Assert::AreEqual(90.0f, resultArr[2], 0.0001f);
		}
	};
}