
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestVC
{
	TEST_CLASS(MatrixTest)
	{
	public:
		TEST_METHOD(Matrix_ExtractProjectionPlane)
		{
			float w = 1280;
			float h = 720;

			float near = 1.5f;
			float far = 1500.0f;
			float aspect = 1.2f;

			Matrix temp[4];
			Matrix::CreatePerspectiveFovLH(temp[0], ToRadian(60), aspect, near, far);
			Matrix::CreatePerspectiveFovRH(temp[1], ToRadian(60), aspect, near, far);
			Matrix::CreateOrthoLH(temp[2], w, h, near, far);
			Matrix::CreateOrthoRH(temp[3], w, h, near, far);

			Assert::AreEqual(near, temp[0].ExtractProjectionNearPlane(), 0.001f);
			Assert::AreEqual(far, temp[0].ExtractProjectionFarPlane(), 0.001f);

			Assert::AreEqual(near, temp[1].ExtractProjectionNearPlane(), 0.001f);
			Assert::AreEqual(far, temp[1].ExtractProjectionFarPlane(), 0.001f);

			Assert::AreEqual(near, temp[2].ExtractProjectionNearPlane(), 0.001f);
			Assert::AreEqual(far, temp[2].ExtractProjectionFarPlane(), 0.001f);

			Assert::AreEqual(near, temp[3].ExtractProjectionNearPlane(), 0.001f);
			Assert::AreEqual(far, temp[3].ExtractProjectionFarPlane(), 0.001f);

			near = 2.5f;
			far = 1750.0f;

			Matrix temp2[4];
			Matrix::CreatePerspectiveFovLH(temp[0], ToRadian(60), aspect, near, far);
			Matrix::CreatePerspectiveFovRH(temp[1], ToRadian(60), aspect, near, far);
			Matrix::CreateOrthoLH(temp[2], w, h, near, far);
			Matrix::CreateOrthoRH(temp[3], w, h, near, far);

			for (Matrix& m : temp)
			{
				m.ReAdjustProjectionNearFarPlane(near, far);
			}

			Assert::IsTrue(temp[0] == temp2[0]);
			Assert::IsTrue(temp[1] == temp2[1]);
			Assert::IsTrue(temp[2] == temp2[2]);
			Assert::IsTrue(temp[3] == temp2[3]);

		}

	};
}