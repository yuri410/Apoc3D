#include <Windows.h>

#include "apoc3d/Engine.h"
#include "apoc3d/IOLib/TaggedData.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/Math/Matrix.h"
#include "apoc3d/Math/Color.h"
#include "apoc3d/Math/Plane.h"
#include "apoc3d/Math/Quaternion.h"
#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/BoundingBox.h"
#include "apoc3d/Math/BoundingSphere.h"
#include "apoc3d/Math/Ray.h"
#include "apoc3d/Math/Viewport.h"
#include "apoc3d/Math/RandomUtils.h"
#include "apoc3d/Utility/Compression.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Vfs/FileSystem.h"

#include <iostream>

using namespace Apoc3D;
using namespace Apoc3D::Math;
using namespace Apoc3D::IO;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;

void TestTaggedData();
void TestListReverse();
void TestListSort();
void TestListSort2();
void TestRLE();
void TestLZ();
void TestMath();

void main()
{
	setlocale(LC_CTYPE, ".ACP");

	//String s = StringUtils::ToString(0.14159265f,8);
	//std::wcout << s.c_str() << L"\n";
	//std::wcout << std::numeric_limits<float>::digits10+1 << L"\n";

	FileSystem::Initialize();
	wchar_t workingDir[260];
	GetCurrentDirectory(260, workingDir);
	FileSystem::getSingleton().AddWrokingDirectory(workingDir);
	
	List<String> items;
	FileSystem::getSingleton().ListDirectoryFiles(L"", items);

	TestMath();
	TestListReverse();
	TestListSort();
	TestListSort2();
	TestTaggedData();
	//TestRLE();

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
	outData->AddEntryRectangle(L"Rectangle", (Apoc3D::Math::Rectangle*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Apoc3D::Math::Rectangle));
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

	inData->GetDataRectangle(L"Rectangle", (Apoc3D::Math::Rectangle*)tempBuffer, sizeof(tempBuffer)/sizeof(Apoc3D::Math::Rectangle));
	CheckEqual((Apoc3D::Math::Rectangle*)tempBuffer, (Apoc3D::Math::Rectangle*)sourceBuffer, sizeof(sourceBuffer)/sizeof(Apoc3D::Math::Rectangle));

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

void TestListReverse()
{
	List<int> subject;

	subject.Add(0);
	subject.Add(1);
	subject.Add(2);
	subject.Add(3);

	subject.Reverse();

	assert(subject[0] == 3);
	assert(subject[1] == 2);
	assert(subject[2] == 1);
	assert(subject[3] == 0);

	subject.Reverse();
	subject.Add(4);
	subject.Reverse();
	assert(subject[0] == 4);
	assert(subject[1] == 3);
	assert(subject[2] == 2);
	assert(subject[3] == 1);
	assert(subject[4] == 0);

}

void TestListSort()
{
	FastList<int> subject(100);
	int counter[1000];
	memset(counter, 0, sizeof(counter));

	for (int i=0;i<100;i++)
	{
		int val = Randomizer::Next(1000);
		subject.Add(val);
		counter[val]++;
	}

	subject.Sort();

	
	for (int i=0;i<subject.getCount()-1;i++)
	{
		if (subject[i+1] < subject[i])
		{
			assert(0);
		}
	}

	int counter2[1000];
	memset(counter2, 0, sizeof(counter2));
	for (int i=0;i<subject.getCount();i++)
	{
		counter2[subject[i]]++;
	}

	assert(memcmp(counter, counter2, sizeof(counter))==0);
}

int comparerTestInt(const int& a, const int& b)
{
	return a - b;
}
void TestListSort2()
{
	FastList<int> subject(100);
	int counter[1000];
	memset(counter, 0, sizeof(counter));

	for (int i=0;i<100;i++)
	{
		int val = Randomizer::Next(1000);
		subject.Add(val);
		counter[val]++;
	}

	subject.Sort(comparerTestInt);


	for (int i=0;i<subject.getCount()-1;i++)
	{
		if (subject[i+1] < subject[i])
		{
			assert(0);
		}
	}

	int counter2[1000];
	memset(counter2, 0, sizeof(counter2));
	for (int i=0;i<subject.getCount();i++)
	{
		counter2[subject[i]]++;
	}

	assert(memcmp(counter, counter2, sizeof(counter))==0);
}

void TestRLE()
{
	FileStream fs(L"Apoc3d.lib");
	char* buffer = new char[(int32)fs.getLength()];
	fs.Read(buffer, fs.getLength());
	fs.Close();

	int32 compressedSize = rleEvalCompressedSize(buffer, (int32)fs.getLength());
	MemoryOutStream compressedStream(compressedSize);
	rleCompress(buffer, (int32)fs.getLength(), &compressedStream);

	assert(compressedStream.getLength() == compressedSize);

	char* compressedData = new char[compressedSize];
	int ret = rleCompress(compressedData, compressedSize, buffer, (int32)fs.getLength());
	assert(ret == compressedSize);

	//////////////////////////////////////////////////////////////////////////

	int32 decompSize = rleEvalDecompressedSize(compressedData, compressedSize);
	assert(decompSize == fs.getLength());

	char* decompressedData = new char[decompSize];
	ret = rleDecompress(decompressedData, decompSize, compressedData, compressedSize);
	assert(ret == decompSize);

	CheckEqual(buffer, decompressedData, decompSize);

	MemoryStream ms(compressedData, compressedSize);
	ret = rleDecompress(decompressedData, decompSize, &ms);
	assert(ret == decompSize);

	CheckEqual(buffer, decompressedData, decompSize);

	delete[] buffer;
	delete[] decompressedData;
}

Matrix* GenerateRandomMatrices(Random& rnd, int32 count)
{
	Matrix* result = new Matrix[count];

	for (int i=0;i<count;i++)
	{
		Matrix& m = result[i];
		Matrix::CreateRotationY(m, rnd.NextFloat() * Math::PI);
	}
	return result;
}

void TestMath()
{
	const int32 count = 1048576; // 64MB * 3

	Random rnd(9934234);

	Matrix* set1 = GenerateRandomMatrices(rnd, count); // 64MB
	Matrix* set2 = GenerateRandomMatrices(rnd, count);
	Matrix* result = GenerateRandomMatrices(rnd, count);

	DWORD start = GetTickCount();
	for (int i=0;i<count;i++)
	{
		Matrix::Multiply(result[i], set1[i], set2[i]);
	}
	DWORD end = GetTickCount();

	DWORD t = end - start;
	std::wcout << t << L"\n";

	BinaryWriter* bw = new BinaryWriter(new FileOutStream(L"testMath1.dat"));
	bw->Write((char*)result, sizeof(Matrix) * count);
	delete bw;
}