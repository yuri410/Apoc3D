
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft
{ 
	namespace VisualStudio
	{ 
		namespace CppUnitTestFramework
		{
			template<> static std::wstring ToString<Vector3>(const Vector3& t) { return t.ToTextString(1); }
			template<> static std::wstring ToString<Point>(const Point& t) { return StringUtils::IntToString(t.X) + L", " + StringUtils::IntToString(t.Y); }
		}
	}
}

namespace UnitTestVC
{
	TEST_CLASS(ConfigurationSectionTest)
	{
	public:
		TEST_METHOD(ConfigSectBool)
		{
			ConfigurationSection* sect = new ConfigurationSection(L"test");

			sect->AddBool(L"A", false);
			sect->AddBool(L"B", true);
			sect->AddAttributeBool(L"A", true);
			sect->AddAttributeBool(L"B", false);

			Assert::IsTrue(sect->GetAttributeBool(L"A"));
			Assert::IsFalse(sect->GetAttributeBool(L"B"));
			Assert::IsFalse(sect->GetBool(L"A"));
			Assert::IsTrue(sect->GetBool(L"B"));

			bool x;
			Assert::IsFalse(sect->TryGetBool(L"C", x));
			Assert::IsFalse(sect->TryGetAttributeBool(L"C", x));

			//Assert::IsTrue(sect->)
			x = false;
			Assert::IsTrue(sect->TryGetBool(L"B", x));
			Assert::IsTrue(x);
			Assert::IsTrue(sect->TryGetAttributeBool(L"B", x));
			Assert::IsFalse(x);

		}

		TEST_METHOD(ConfigSectInt)
		{
			ConfigurationSection* sect = new ConfigurationSection(L"test");

			sect->AddInt(L"A", 1);
			sect->AddInt(L"B", 2);
			sect->AddAttributeInt(L"A", 2);
			sect->AddAttributeInt(L"B", 1);

			Assert::AreEqual(2, sect->GetAttributeInt(L"A"));
			Assert::AreEqual(1, sect->GetAttributeInt(L"B"));
			Assert::AreEqual(1, sect->GetInt(L"A"));
			Assert::AreEqual(2, sect->GetInt(L"B"));

			int32 x;
			Assert::IsFalse(sect->TryGetInt(L"C", x));
			Assert::IsFalse(sect->TryGetAttributeInt(L"C", x));

			x = 0;
			Assert::IsTrue(sect->TryGetInt(L"B", x));
			Assert::AreEqual(2, x);
			Assert::IsTrue(sect->TryGetAttributeInt(L"B", x));
			Assert::AreEqual(1, x);

			int32 vals[3] = { 3, 4, 5};
			sect->AddInts(L"C", vals, 3);
			sect->AddAttributeInts(L"E", vals, 3);

			FastList<int32> buf;
			Assert::IsFalse(sect->TryGetInts(L"D", buf));

			buf.Clear();
			sect->GetInts(L"C", buf);
			Assert::AreEqual(3, buf.getCount());
			Assert::AreEqual(3, buf[0]);
			Assert::AreEqual(4, buf[1]);
			Assert::AreEqual(5, buf[2]);

			buf.Clear();
			Assert::IsTrue(sect->TryGetInts(L"C", buf));
			Assert::AreEqual(3, buf.getCount());
			Assert::AreEqual(3, buf[0]);
			Assert::AreEqual(4, buf[1]);
			Assert::AreEqual(5, buf[2]);

			buf.Clear();
			Assert::IsTrue(sect->TryGetAttributeInts(L"E", buf));
			Assert::AreEqual(3, buf.getCount());
			Assert::AreEqual(3, buf[0]);
			Assert::AreEqual(4, buf[1]);
			Assert::AreEqual(5, buf[2]);

			buf.Clear();
			x = 0;
			Assert::IsTrue(sect->TryGetAttributeInts(L"E", vals, 3, &x));
			Assert::AreEqual(3, x);
			Assert::AreEqual(3, vals[0]);
			Assert::AreEqual(4, vals[1]);
			Assert::AreEqual(5, vals[2]);

		}

		TEST_METHOD(ConfigSectVector3)
		{
			ConfigurationSection* sect = new ConfigurationSection(L"test");

			const Vector3 vecVal = Vector3(1,2,3);

			sect->AddVector3(L"A", vecVal);
			sect->AddAttributeVector3(L"B", vecVal);

			Assert::AreEqual(vecVal, sect->GetAttributeVector3(L"B"));
			Assert::AreEqual(vecVal, sect->GetVector3(L"A"));

			const Vector3 vals[3] = { vecVal, -vecVal, vecVal*2 };
			sect->AddVector3s(L"C", vals, 3);
			sect->AddAttributeVector3s(L"E", vals, 3);

			FastList<Vector3> buf;
			Assert::IsFalse(sect->TryGetVector3s(L"D", buf));

			buf.Clear();
			sect->GetVector3s(L"C", buf);
			Assert::AreEqual(3, buf.getCount());
			for (int32 i=0;i<3;i++)
				Assert::AreEqual(vals[i], vals[i]);

			buf.Clear();
			Assert::IsTrue(sect->TryGetVector3s(L"C", buf));
			Assert::AreEqual(3, buf.getCount());
			for (int32 i=0;i<3;i++)
				Assert::AreEqual(vals[i], vals[i]);

			buf.Clear();
			Assert::IsTrue(sect->TryGetAttributeVector3s(L"E", buf));
			Assert::AreEqual(3, buf.getCount());
			for (int32 i=0;i<3;i++)
				Assert::AreEqual(vals[i], vals[i]);

			int32 num = 0;
			Vector3 resVals[3];
			Assert::IsTrue(sect->TryGetAttributeVector3s(L"E", resVals, 3, &num));
			Assert::AreEqual(3, num);
			for (int32 i=0;i<3;i++)
				Assert::AreEqual(vals[i], vals[i]);
		}

		TEST_METHOD(ConfigSectPoint)
		{
			ConfigurationSection* sect = new ConfigurationSection(L"test");

			const Point vecVal = Point(1,2);

			sect->AddPoint(L"A", vecVal);
			sect->AddAttributePoint(L"B", vecVal);

			Assert::AreEqual(vecVal, sect->GetAttributePoint(L"B"));
			Assert::AreEqual(vecVal, sect->GetPoint(L"A"));

			const Point vals[3] = { Point(1,2), Point(3,-4), Point(-2,5) };
			sect->AddPoints(L"C", vals, 3);
			sect->AddAttributePoints(L"E", vals, 3);

			FastList<Point> buf;
			Assert::IsFalse(sect->TryGetPoints(L"D", buf));

			buf.Clear();
			sect->GetPoints(L"C", buf);
			Assert::AreEqual(3, buf.getCount());
			for (int32 i=0;i<3;i++)
				Assert::AreEqual(vals[i], vals[i]);

			buf.Clear();
			Assert::IsTrue(sect->TryGetPoints(L"C", buf));
			Assert::AreEqual(3, buf.getCount());
			for (int32 i=0;i<3;i++)
				Assert::AreEqual(vals[i], vals[i]);

			buf.Clear();
			Assert::IsTrue(sect->TryGetAttributePoints(L"E", buf));
			Assert::AreEqual(3, buf.getCount());
			for (int32 i=0;i<3;i++)
				Assert::AreEqual(vals[i], vals[i]);

			int32 num = 0;
			Point resVals[3];
			Assert::IsTrue(sect->TryGetAttributePoints(L"E", resVals, 3, &num));
			Assert::AreEqual(3, num);
			for (int32 i=0;i<3;i++)
				Assert::AreEqual(vals[i], vals[i]);

		}
	};
}