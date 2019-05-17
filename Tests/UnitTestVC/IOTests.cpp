
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestVC
{
	TEST_CLASS(IOTest)
	{
	public:
		TEST_METHOD(IOUtils_Primitives)
		{
			int16 i16 = -12345;
			uint16 u16 = 12345;

			int32 i32 = -123456789;
			uint32 u32 = 123456789;

			int64 i64 = -1234567890123;
			uint64 u64 = 1234567890123;

			float f32 = 12345;
			double f64 = 12345678;

			char buf[8];

			i16_mb_le(i16, buf);
			Assert::AreEqual(mb_i16_le(buf), i16);

			i16_mb_be(u16, buf);
			Assert::AreEqual(mb_u16_be(buf), u16);

			i32_mb_le(i32, buf);
			Assert::AreEqual(mb_i32_le(buf), i32);

			i32_mb_be(u32, buf);
			Assert::AreEqual(mb_u32_be(buf), u32);

			i32_mb_le(i64, buf);
			Assert::AreEqual(mb_i64_le(buf), i64);

			i32_mb_be(u64, buf);
			Assert::AreEqual(mb_u64_be(buf), u64);

			f32_mb_le(f32, buf);
			Assert::AreEqual(mb_f32_le(buf), f32);

			f64_mb_be(f64, buf);
			Assert::AreEqual(mb_f64_be(buf), f64);

			//String result = PathUtils::GetDirectory(LR"(F:\Temp\ipch\\\0bulletsolution-2facbcfd\.sdf)");

			//Assert::AreEqual(String(LR"(F:\Temp\ipch\0bulletsolution-2facbcfd)"), result);
		}

	};
}