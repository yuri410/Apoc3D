
#include "apoc3d/Engine.h"
#include "apoc3d/IOLib/TaggedData.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Math/Matrix.h"
#include "apoc3d/Math/Color.h"
#include "apoc3d/Math/Plane.h"
#include "apoc3d/Math/Quaternion.h"
#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/BoundingBox.h"
#include "apoc3d/Math/BoundingSphere.h"
#include "apoc3d/Math/Ray.h"
#include "apoc3d/Math/Viewport.h"

using namespace Apoc3D;
using namespace Apoc3D::Math;
using namespace Apoc3D::IO;
using namespace Apoc3D::VFS;

void TestTaggedData();

void main()
{
	
	TestTaggedData();
}

template <typename T>
void CheckEqual(const T* a, const T* b, int count)
{
	for (int i=0;i<count;i++)
	{
		if (a[i] != b[i])
		{
			throw std::exception();
		}
	}
}

void TestTaggedData()
{
	char sourceBuffer[1024];
	for (int i=0;i<1024;i++) sourceBuffer[i] = (char)(i % 3);

	TaggedDataWriter* outData = new TaggedDataWriter(true);
	//outData->AddEntryBool(L"Bool", (bool*)sourceBuffer, sizeof(sourceBuffer)/sizeof(bool));
	outData->AddEntryBoundingBox(L"BoundingBox", (BoundingBox*)sourceBuffer, sizeof(sourceBuffer)/sizeof(BoundingBox));
	outData->AddEntryBoundingSphere(L"BoundingSphere", (BoundingSphere*)sourceBuffer, sizeof(sourceBuffer)/sizeof(BoundingSphere));
	outData->AddEntryColor4(L"Color4", (Color4*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Color4));
	outData->AddEntryDouble(L"Double", (double*)sourceBuffer, sizeof(sourceBuffer)/sizeof(double));
	outData->AddEntryInt16(L"Int16", (int16*)sourceBuffer, sizeof(sourceBuffer)/sizeof(int16));
	outData->AddEntryInt32(L"Int32", (int32*)sourceBuffer, sizeof(sourceBuffer)/sizeof(int32));
	outData->AddEntryInt64(L"Int64", (int64*)sourceBuffer, sizeof(sourceBuffer)/sizeof(int64));
	outData->AddEntryMathSize(L"MathSize", (Size*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Size));
	outData->AddEntryMatrix(L"Matrix", (Matrix*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Matrix));
	outData->AddEntryPlane(L"Plane", (Plane*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Plane));
	outData->AddEntryPoint(L"Point", (Point*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Point));
	outData->AddEntryPointF(L"PointF", (PointF*)sourceBuffer, sizeof(sourceBuffer)/sizeof(PointF));
	outData->AddEntryQuaternion(L"Quaternion", (Quaternion*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Quaternion));
	outData->AddEntryRay(L"Ray", (Ray*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Ray));
	outData->AddEntryRectangle(L"Rectangle", (Rectangle*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Rectangle));
	outData->AddEntryRectangleF(L"RectangleF", (RectangleF*)sourceBuffer, sizeof(sourceBuffer)/sizeof(RectangleF));
	outData->AddEntrySingle(L"Single", (float*)sourceBuffer, sizeof(sourceBuffer)/sizeof(float));
	outData->AddEntryUInt16(L"UInt16", (uint16*)sourceBuffer, sizeof(sourceBuffer)/sizeof(uint16));
	outData->AddEntryUInt32(L"UInt32", (uint32*)sourceBuffer, sizeof(sourceBuffer)/sizeof(uint32));
	outData->AddEntryUInt64(L"UInt64", (uint64*)sourceBuffer, sizeof(sourceBuffer)/sizeof(uint64));
	outData->AddEntryVector2(L"Vector2", (Vector2*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Vector2));
	outData->AddEntryVector3(L"Vector3", (Vector3*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Vector3));
	outData->AddEntryVector4(L"Vector4", (Vector4*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Vector4));
	outData->AddEntryViewport(L"Viewport", (Viewport*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Viewport));

	MemoryOutStream* buffer = new MemoryOutStream(0xffff);
	outData->Save(new VirtualStream(buffer));
	buffer->setPosition(0);
	char tempBuffer[1024];
	TaggedDataReader* inData = new TaggedDataReader(buffer);
	//inData->GetDataBool(L"Bool", (bool*)tempBuffer, sizeof(tempBuffer)/sizeof(bool));
	//CheckEqual((bool*)tempBuffer, (bool*)sourceBuffer, sizeof(sourceBuffer)/sizeof(bool));
	
	inData->GetDataBoundingBox(L"BoundingBox", (BoundingBox*)tempBuffer, sizeof(tempBuffer)/sizeof(BoundingBox));
	CheckEqual((BoundingBox*)tempBuffer, (BoundingBox*)sourceBuffer, sizeof(sourceBuffer)/sizeof(BoundingBox));

	inData->GetDataBoundingSphere(L"BoundingSphere", (BoundingSphere*)tempBuffer, sizeof(tempBuffer)/sizeof(BoundingSphere));
	CheckEqual((BoundingSphere*)tempBuffer, (BoundingSphere*)sourceBuffer, sizeof(sourceBuffer)/sizeof(BoundingSphere));

	inData->GetDataColor4(L"Color4", (Color4*)tempBuffer, sizeof(tempBuffer)/sizeof(Color4));
	CheckEqual((Color4*)tempBuffer, (Color4*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Color4));

	inData->GetDataDouble(L"Double", (double*)tempBuffer, sizeof(tempBuffer)/sizeof(double));
	CheckEqual((double*)tempBuffer, (double*)sourceBuffer, sizeof(sourceBuffer)/sizeof(double));

	inData->GetDataInt16(L"Int16", (int16*)tempBuffer, sizeof(tempBuffer)/sizeof(int16));
	CheckEqual((int16*)tempBuffer, (int16*)sourceBuffer, sizeof(sourceBuffer)/sizeof(int16));

	inData->GetDataInt32(L"Int32", (int32*)tempBuffer, sizeof(tempBuffer)/sizeof(int32));
	CheckEqual((int32*)tempBuffer, (int32*)sourceBuffer, sizeof(sourceBuffer)/sizeof(int32));

	inData->GetDataInt64(L"Int64", (int64*)tempBuffer, sizeof(tempBuffer)/sizeof(int64));
	CheckEqual((int64*)tempBuffer, (int64*)sourceBuffer, sizeof(sourceBuffer)/sizeof(int64));

	inData->GetDataMathSize(L"MathSize", (Size*)tempBuffer, sizeof(tempBuffer)/sizeof(Size));
	CheckEqual((Size*)tempBuffer, (Size*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Size));

	inData->GetDataMatrix(L"Matrix", (Matrix*)tempBuffer, sizeof(tempBuffer)/sizeof(Matrix));
	CheckEqual((Matrix*)tempBuffer, (Matrix*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Matrix));

	inData->GetDataPlane(L"Plane", (Plane*)tempBuffer, sizeof(tempBuffer)/sizeof(Plane));
	CheckEqual((Plane*)tempBuffer, (Plane*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Plane));

	inData->GetDataPoint(L"Point", (Point*)tempBuffer, sizeof(tempBuffer)/sizeof(Point));
	CheckEqual((Point*)tempBuffer, (Point*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Point));

	inData->GetDataPointF(L"PointF", (PointF*)tempBuffer, sizeof(tempBuffer)/sizeof(PointF));
	CheckEqual((PointF*)tempBuffer, (PointF*)sourceBuffer, sizeof(sourceBuffer)/sizeof(PointF));

	inData->GetDataQuaternion(L"Quaternion", (Quaternion*)tempBuffer, sizeof(tempBuffer)/sizeof(Quaternion));
	CheckEqual((Quaternion*)tempBuffer, (Quaternion*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Quaternion));

	inData->GetDataRay(L"Ray", (Ray*)tempBuffer, sizeof(tempBuffer)/sizeof(Ray));
	CheckEqual((Ray*)tempBuffer, (Ray*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Ray));

	inData->GetDataRectangle(L"Rectangle", (Rectangle*)tempBuffer, sizeof(tempBuffer)/sizeof(Rectangle));
	CheckEqual((Rectangle*)tempBuffer, (Rectangle*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Rectangle));

	inData->GetDataRectangleF(L"RectangleF", (RectangleF*)tempBuffer, sizeof(tempBuffer)/sizeof(RectangleF));
	CheckEqual((RectangleF*)tempBuffer, (RectangleF*)sourceBuffer, sizeof(sourceBuffer)/sizeof(RectangleF));

	inData->GetDataSingle(L"Single", (float*)tempBuffer, sizeof(tempBuffer)/sizeof(float));
	CheckEqual((float*)tempBuffer, (float*)sourceBuffer, sizeof(sourceBuffer)/sizeof(float));

	inData->GetDataUInt16(L"UInt16", (uint16*)tempBuffer, sizeof(tempBuffer)/sizeof(uint16));
	CheckEqual((uint16*)tempBuffer, (uint16*)sourceBuffer, sizeof(sourceBuffer)/sizeof(uint16));

	inData->GetDataUInt32(L"UInt32", (uint32*)tempBuffer, sizeof(tempBuffer)/sizeof(uint32));
	CheckEqual((uint32*)tempBuffer, (uint32*)sourceBuffer, sizeof(sourceBuffer)/sizeof(uint32));

	inData->GetDataUInt64(L"UInt64", (uint64*)tempBuffer, sizeof(tempBuffer)/sizeof(uint64));
	CheckEqual((uint64*)tempBuffer, (uint64*)sourceBuffer, sizeof(sourceBuffer)/sizeof(uint64));

	inData->GetDataVector2(L"Vector2", (Vector2*)tempBuffer, sizeof(tempBuffer)/sizeof(Vector2));
	CheckEqual((Vector2*)tempBuffer, (Vector2*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Vector2));

	inData->GetDataVector3(L"Vector3", (Vector3*)tempBuffer, sizeof(tempBuffer)/sizeof(Vector3));
	CheckEqual((Vector3*)tempBuffer, (Vector3*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Vector3));

	inData->GetDataVector4(L"Vector4", (Vector4*)tempBuffer, sizeof(tempBuffer)/sizeof(Vector4));
	CheckEqual((Vector4*)tempBuffer, (Vector4*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Vector4));

	inData->GetDataViewport(L"Viewport", (Viewport*)tempBuffer, sizeof(tempBuffer)/sizeof(Viewport));
	CheckEqual((Viewport*)tempBuffer, (Viewport*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Viewport));

	delete inData;
	delete outData;
}

