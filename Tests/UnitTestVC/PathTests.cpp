
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestVC
{
	TEST_CLASS(PathUtilsTest)
	{
	public:
		TEST_METHOD(PathUtils_GetDirectory)
		{
			String result = PathUtils::GetDirectory(LR"(F:\Temp\ipch\\\0bulletsolution-2facbcfd\.sdf)");

			Assert::AreEqual(String(LR"(F:\Temp\ipch\0bulletsolution-2facbcfd)"), result);
		}
		
		TEST_METHOD(PathUtils_GetFileNameNoExt)
		{
			String result = PathUtils::GetFileNameNoExt(LR"(F:\Temp/\/\\ipch\0bulletsolution-2facbcfd\b.sdf)");

			Assert::AreEqual(String(L"b"), result);
		}
		
		TEST_METHOD(PathUtils_GetFileName)
		{
			String result = PathUtils::GetFileName(LR"(F:\Temp\ipch\0bulletsolution-2facbcfd\ .sdf)");

			Assert::AreEqual(String(L" .sdf"), result);
		}
		
		TEST_METHOD(PathUtils_NormalizePath)
		{
			String result = PathUtils::NormalizePath(LR"(F:/Temp///ipch\\\\0bulletsolution-2facbcfd)");

			Assert::AreEqual(String(LR"(F:\Temp\ipch\0bulletsolution-2facbcfd)"), result);
		}
		
		TEST_METHOD(PathUtils_Split)
		{
			List<String> parts = PathUtils::Split(LR"(F:/temp//ipch\\\0bulletsolution-2facbcfd\\\temp.dat)");
			Assert::AreEqual(4, parts.getCount());
		}

		TEST_METHOD(PathUtils_GetCommonBasePath)
		{
			String cbp;
			bool r = PathUtils::GetCommonBasePath(LR"(F:/Temp/ipch/0bulletsolution-2facbcfd/temp.dat)", LR"(F:/Temp/ipch/1bulletsolution-2facbcfd/temp.dat)", cbp);

			Assert::IsTrue(r);
			Assert::AreEqual(String(L"F:/Temp/ipch"), cbp);
		}

		TEST_METHOD(PathUtils_GetRelativePath)
		{
			String rel;
			bool r = PathUtils::GetRelativePath(LR"(F:/Temp/ipch/0bulletsolution-2facbcfd/temp.dat)", LR"(F:/Temp/ipch/1bulletsolution-2facbcfd/temp.dat)", rel);

			Assert::IsTrue(r);
			Assert::AreEqual(String(L"..\\..\\1bulletsolution-2facbcfd\\temp.dat"), rel);
		}

		
		TEST_METHOD(PathUtils_Match)
		{
			bool result = PathUtils::Match(LR"(F:/Temp/ipch/\0bulletsolution-2facbcfd/temp.dat)", LR"(F:/temp/ipch\/*-*\*.dat)");
			Assert::IsTrue(result);
		}
		
		TEST_METHOD(PathUtils_ComparePath)
		{
			bool result = PathUtils::ComparePath(LR"(F:\Temp\ipch\0bulletsolution-2facbcfd\temp.dat)", LR"(F:/temp//ipch\\\0bulletsolution-2facbcfd\\\temp.dat)");
			Assert::IsTrue(result);

			result = PathUtils::ComparePath(LR"(F:\Temp\ipch\0bulletsolution-2facbcfb\temp.dat)", LR"(F:/temp//ipch\\\0bulletsolution-2facbcfd\\\temp.dat)");
			Assert::IsFalse(result);
		}
		
		TEST_METHOD(PathUtils_CountPathLevels)
		{
			int32 partCount = PathUtils::CountPathLevels(LR"(F:/Temp/ipch\0bulletsolution-2facbcfd\temp.dat)");
			Assert::AreEqual(4, partCount); // 4 levels
		}

	};
}