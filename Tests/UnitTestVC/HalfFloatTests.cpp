
struct DataPair
{
	uint16 r16;
	uint32 r32;
};

const DataPair r16r32[] = 
{
#include "data\halfTestR16R32.txt"
};
const DataPair r32r16[] =
{
#include "data\halfTestR32R16.txt"
};

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft
{
	namespace VisualStudio
	{
		namespace CppUnitTestFramework
		{
			template<> static std::wstring ToString<uint16>(const uint16& t) { return StringUtils::IntToString(t); }
		}
	}
}

namespace UnitTestVC
{
	TEST_CLASS(HalfFloatTest)
	{
	public:
		TEST_METHOD(HalfFloat_R32ToR16)
		{
			for (const auto& e : r32r16)
			{
				uint16 iv = Math::R32ToR16I(e.r32);

				Assert::AreEqual(e.r16, iv);
			}
			
		}

		TEST_METHOD(HalfFloat_R16ToR32)
		{
			for (const auto& e : r16r32)
			{
				uint32 iv = Math::R16ToR32I(e.r16);

				if (e.r32 != iv)
				{
					Assert::Fail();
				}

				Assert::AreEqual(e.r32, iv);
			}

		}

	};
}