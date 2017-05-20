using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestVC
{
	TEST_CLASS(TaggedDataTest)
	{
	public:
		
		template <typename T, int32 N>
		bool CheckEqual(const char (&_a)[N], const char (&_b)[N])
		{
			const T* a = (const T*)_a;
			const T* b = (const T*)_b;

			int32 count = N / sizeof(T);
			for (int i = 0; i < count; i++)
			{
				if (a[i] != b[i])
				{
					return false;
				}
			}
			return true;
		}
		template <typename T, int32 N>
		bool CheckEqual(const T (&a)[N], const T (&b)[N])
		{
			for (int i = 0; i < N; i++)
			{
				if (a[i] != b[i])
				{
					return false;
				}
			}
			return true;
		}

		TEST_METHOD(TaggedDataTest_General)
		{
			char sourceBuffer[1024];
			for (int i = 0; i < 1024; i++) sourceBuffer[i] = (char)(i % 3);

			bool boolBuffer[1024];
			for (bool& b : boolBuffer)
				b = Randomizer::NextBool();

			TaggedDataWriter outData(true);
			outData.AddBool("Bool", boolBuffer);
			outData.AddBoundingBox("BoundingBox", (BoundingBox*)sourceBuffer, sizeof(sourceBuffer) / sizeof(BoundingBox));
			outData.AddBoundingSphere("BoundingSphere", (BoundingSphere*)sourceBuffer, sizeof(sourceBuffer) / sizeof(BoundingSphere));
			outData.AddColor4("Color4", (Color4*)sourceBuffer, sizeof(sourceBuffer) / sizeof(Color4));
			outData.AddDouble("Double", (double*)sourceBuffer, sizeof(sourceBuffer) / sizeof(double));
			outData.AddInt16("Int16", (int16*)sourceBuffer, sizeof(sourceBuffer) / sizeof(int16));
			outData.AddInt32("Int32", (int32*)sourceBuffer, sizeof(sourceBuffer) / sizeof(int32));
			outData.AddInt64("Int64", (int64*)sourceBuffer, sizeof(sourceBuffer) / sizeof(int64));
			outData.AddMathSize("MathSize", (Size*)sourceBuffer, sizeof(sourceBuffer) / sizeof(Size));
			outData.AddMatrix("Matrix", (Matrix*)sourceBuffer, sizeof(sourceBuffer) / sizeof(Matrix));
			outData.AddPlane("Plane", (Plane*)sourceBuffer, sizeof(sourceBuffer) / sizeof(Plane));
			outData.AddPoint("Point", (Point*)sourceBuffer, sizeof(sourceBuffer) / sizeof(Point));
			outData.AddPointF("PointF", (PointF*)sourceBuffer, sizeof(sourceBuffer) / sizeof(PointF));
			outData.AddQuaternion("Quaternion", (Quaternion*)sourceBuffer, sizeof(sourceBuffer) / sizeof(Quaternion));
			outData.AddRay("Ray", (Ray*)sourceBuffer, sizeof(sourceBuffer) / sizeof(Ray));
			outData.AddRectangle("Rectangle", (Apoc3D::Math::Rectangle*)sourceBuffer, sizeof(sourceBuffer) / sizeof(Apoc3D::Math::Rectangle));
			outData.AddRectangleF("RectangleF", (RectangleF*)sourceBuffer, sizeof(sourceBuffer) / sizeof(RectangleF));
			outData.AddSingle("Single", (float*)sourceBuffer, sizeof(sourceBuffer) / sizeof(float));
			outData.AddUInt16("UInt16", (uint16*)sourceBuffer, sizeof(sourceBuffer) / sizeof(uint16));
			outData.AddUInt32("UInt32", (uint32*)sourceBuffer, sizeof(sourceBuffer) / sizeof(uint32));
			outData.AddUInt64("UInt64", (uint64*)sourceBuffer, sizeof(sourceBuffer) / sizeof(uint64));
			outData.AddVector2("Vector2", (Vector2*)sourceBuffer, sizeof(sourceBuffer) / sizeof(Vector2));
			outData.AddVector3("Vector3", (Vector3*)sourceBuffer, sizeof(sourceBuffer) / sizeof(Vector3));
			outData.AddVector4("Vector4", (Vector4*)sourceBuffer, sizeof(sourceBuffer) / sizeof(Vector4));
			outData.AddViewport("Viewport", (Viewport*)sourceBuffer, sizeof(sourceBuffer) / sizeof(Viewport));

			MemoryOutStream buffer(0xffff);
			outData.Save(buffer);
			buffer.setPosition(0);

			char tempBuffer[1024];
			bool tempBool[1024];

			TaggedDataReader inData(&buffer);
			inData.SuspendStreamRelease();
			inData.GetBool("Bool", tempBool);
			Assert::IsTrue(CheckEqual(tempBool, boolBuffer));

			inData.GetBoundingBox("BoundingBox", (BoundingBox*)tempBuffer, sizeof(tempBuffer) / sizeof(BoundingBox));
			Assert::IsTrue(CheckEqual<BoundingBox>(tempBuffer, sourceBuffer));

			inData.GetBoundingSphere("BoundingSphere", (BoundingSphere*)tempBuffer, sizeof(tempBuffer) / sizeof(BoundingSphere));
			Assert::IsTrue(CheckEqual<BoundingSphere>(tempBuffer, sourceBuffer));

			inData.GetColor4("Color4", (Color4*)tempBuffer, sizeof(tempBuffer) / sizeof(Color4));
			Assert::IsTrue(CheckEqual<Color4>(tempBuffer, sourceBuffer));

			inData.GetDouble("Double", (double*)tempBuffer, sizeof(tempBuffer) / sizeof(double));
			Assert::IsTrue(CheckEqual<double>(tempBuffer, sourceBuffer));

			inData.GetInt16("Int16", (int16*)tempBuffer, sizeof(tempBuffer) / sizeof(int16));
			Assert::IsTrue(CheckEqual<int16>(tempBuffer, sourceBuffer));

			inData.GetInt32("Int32", (int32*)tempBuffer, sizeof(tempBuffer) / sizeof(int32));
			Assert::IsTrue(CheckEqual<int32>(tempBuffer, sourceBuffer));

			inData.GetInt64("Int64", (int64*)tempBuffer, sizeof(tempBuffer) / sizeof(int64));
			Assert::IsTrue(CheckEqual<int64>(tempBuffer, sourceBuffer));

			inData.GetMathSize("MathSize", (Size*)tempBuffer, sizeof(tempBuffer) / sizeof(Size));
			Assert::IsTrue(CheckEqual<Size>(tempBuffer, sourceBuffer));

			inData.GetMatrix("Matrix", (Matrix*)tempBuffer, sizeof(tempBuffer) / sizeof(Matrix));
			Assert::IsTrue(CheckEqual<Matrix>(tempBuffer, sourceBuffer));

			inData.GetPlane("Plane", (Plane*)tempBuffer, sizeof(tempBuffer) / sizeof(Plane));
			Assert::IsTrue(CheckEqual<Plane>(tempBuffer, sourceBuffer));

			inData.GetPoint("Point", (Point*)tempBuffer, sizeof(tempBuffer) / sizeof(Point));
			Assert::IsTrue(CheckEqual<Point>(tempBuffer, sourceBuffer));

			inData.GetPointF("PointF", (PointF*)tempBuffer, sizeof(tempBuffer) / sizeof(PointF));
			Assert::IsTrue(CheckEqual<PointF>(tempBuffer, sourceBuffer));

			inData.GetQuaternion("Quaternion", (Quaternion*)tempBuffer, sizeof(tempBuffer) / sizeof(Quaternion));
			Assert::IsTrue(CheckEqual<Quaternion>(tempBuffer, sourceBuffer));

			inData.GetRay("Ray", (Ray*)tempBuffer, sizeof(tempBuffer) / sizeof(Ray));
			Assert::IsTrue(CheckEqual<Ray>(tempBuffer, sourceBuffer));

			inData.GetRectangle("Rectangle", (Apoc3D::Math::Rectangle*)tempBuffer, sizeof(tempBuffer) / sizeof(Apoc3D::Math::Rectangle));
			Assert::IsTrue(CheckEqual<Apoc3D::Math::Rectangle>(tempBuffer, sourceBuffer));

			inData.GetRectangleF("RectangleF", (RectangleF*)tempBuffer, sizeof(tempBuffer) / sizeof(RectangleF));
			Assert::IsTrue(CheckEqual<RectangleF>(tempBuffer, sourceBuffer));

			inData.GetSingle("Single", (float*)tempBuffer, sizeof(tempBuffer) / sizeof(float));
			Assert::IsTrue(CheckEqual<float>(tempBuffer, sourceBuffer));

			inData.GetUInt16("UInt16", (uint16*)tempBuffer, sizeof(tempBuffer) / sizeof(uint16));
			Assert::IsTrue(CheckEqual<uint16>(tempBuffer, sourceBuffer));

			inData.GetUInt32("UInt32", (uint32*)tempBuffer, sizeof(tempBuffer) / sizeof(uint32));
			Assert::IsTrue(CheckEqual<uint32>(tempBuffer, sourceBuffer));

			inData.GetUInt64("UInt64", (uint64*)tempBuffer, sizeof(tempBuffer) / sizeof(uint64));
			Assert::IsTrue(CheckEqual<uint64>(tempBuffer, sourceBuffer));

			inData.GetVector2("Vector2", (Vector2*)tempBuffer, sizeof(tempBuffer) / sizeof(Vector2));
			Assert::IsTrue(CheckEqual<Vector2>(tempBuffer, sourceBuffer));

			inData.GetVector3("Vector3", (Vector3*)tempBuffer, sizeof(tempBuffer) / sizeof(Vector3));
			Assert::IsTrue(CheckEqual<Vector3>(tempBuffer, sourceBuffer));

			inData.GetVector4("Vector4", (Vector4*)tempBuffer, sizeof(tempBuffer) / sizeof(Vector4));
			Assert::IsTrue(CheckEqual<Vector4>(tempBuffer, sourceBuffer));

			inData.GetViewport("Viewport", (Viewport*)tempBuffer, sizeof(tempBuffer) / sizeof(Viewport));
			Assert::IsTrue(CheckEqual<Viewport>(tempBuffer, sourceBuffer));

		}


	};

	TEST_CLASS(TaggedDataKeyTest)
	{
	public:

		TEST_METHOD(TaggedDataKeyTest_General)
		{
			TaggedDataKey k1 = "sdsartre_2";
			TaggedDataKey k2 = TaggedDataKey("sdsartre_") + "2";
			TaggedDataKey k3 = TaggedDataKey(std::string("sdsartre_")) + "2";
			TaggedDataKey k4 = TaggedDataKey("sdsartre_") + 2;

			Assert::IsTrue(k1 == k2);
			Assert::IsTrue(k1 == k3);
			Assert::IsTrue(k1 == k4);

			TaggedDataKey k5 = std::string("sdsartre_0_fdshfiedsfsdlnsdsafewnrkwenrkejwenrkjwenrkesadsatretreterwnkrjew");
			TaggedDataKey k6 = TaggedDataKey("sdsartre_0_") + "fdshfiedsfsdlnsdsafewnrkwenrkejwenrkjwenrkesadsatretreterwnkrjew";
			Assert::IsTrue(k5 == k6);

			TaggedDataKey* keys[] = { &k1,&k2,&k3,&k4,&k5,&k6 };
			for (TaggedDataKey* k : keys)
			{
				MemoryOutStream buf(1024);
				BinaryWriter bw(&buf, false);
				k->Write(bw);

				MemoryStream ms(buf.getDataPointer(), buf.getLength());
				BinaryReader br(&ms, false);
				TaggedDataKey nk;
				nk.Read(br);

				Assert::IsTrue(*k == nk);
			}
			
		}

		TEST_METHOD(TaggedDataKeyTest_Append)
		{
			{
				TaggedDataKey k1 = "gfhtyu_fddsfsd";
				TaggedDataKey k2 = TaggedDataKey("gfhtyu_") + "fddsfsd";
				TaggedDataKey k3 = TaggedDataKey(std::string("gfhtyu_")) + "fddsfsd";

				Assert::IsTrue(k1 == k2);
				Assert::IsTrue(k1 == k3);
			}

			Random rnd(888);
			for (int32 i = 0; i < 10000; i++)
			{
				TaggedDataKey k1 = "gfhtyu_fddsfsd_";

				int32 appendLen = rnd.Next(4, 64);

				std::string ap(appendLen, ' ');

				for (char& c : ap)
					c = rnd.Next('A', 'Z');

				TaggedDataKey k2 = k1 + ap;
				TaggedDataKey k3 = (std::string("gfhtyu_fddsfsd_") + ap);
				Assert::IsTrue(k2 == k3);

				int32 ap2 = abs(rnd.Next());
				TaggedDataKey k4 = k1 + ap2;
				TaggedDataKey k5 = (std::string("gfhtyu_fddsfsd_") + StringUtils::IntToNarrowString(ap2));
				Assert::IsTrue(k4 == k5);
			}
		}
	};
}