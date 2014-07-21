#include <Windows.h>

#include "apoc3d/Engine.h"
#include "apoc3d/Config/IniConfigurationFormat.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"
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
#include "apoc3d/Library/lz4.h"
#include "apoc3d/Library/lz4hc.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/FileLocateRule.h"

#include <iostream>

using namespace Apoc3D;
using namespace Apoc3D::Config;
using namespace Apoc3D::Math;
using namespace Apoc3D::IO;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;

void TestTaggedData();
void TestRLE();
void TestMath();
void TestBufferStream();
void TestLZ4();

void TestIni();
void TestXml();

void TestRandom();

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
	TestTaggedData();
	TestBufferStream();
	//TestRLE();
	//TestLZ4();
	TestIni();
	TestXml();

	//TestRandom();

	
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


int comparerTestInt(const int& a, const int& b)
{
	return a - b;
}
void TestListSort2()
{
	List<int> subject(100);
	int counter[1000];
	memset(counter, 0, sizeof(counter));

	for (int i=0;i<100;i++)
	{
		int val = Randomizer::NextExclusive(1000);
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

void memcpytest(char* dst, char* src, int32 count)
{
#if defined(__GNUC__)  && !defined(LZ4_FORCE_UNALIGNED_ACCESS)
#  define _PACKED __attribute__ ((packed))
#else
#  define _PACKED
#endif

	typedef struct { uint32 v; }  _PACKED U32_S;
	typedef struct {size_t v;} _PACKED size_t_S;

#define STEPSIZE sizeof(size_t)
#define A32(x)   (((U32_S *)(x))->v)
#define AARCH(x) (((size_t_S *)(x))->v)

#define LZ4_COPYSTEP(d,s)         { AARCH(d) = AARCH(s); d+=STEPSIZE; s+=STEPSIZE; }
#define LZ4_COPY8(d,s)            { LZ4_COPYSTEP(d,s); if (STEPSIZE<8) LZ4_COPYSTEP(d,s); }

#if LZ4_ARCH64 || !defined(__GNUC__)
#  define LZ4_WILDCOPY(d,s,e)     { do { LZ4_COPY8(d,s) } while (d<e); }           /* at the end, d>=e; */
#else
#  define LZ4_WILDCOPY(d,s,e)     { if (likely(e-d <= 8)) LZ4_COPY8(d,s) else do { LZ4_COPY8(d,s) } while (d<e); }
#endif
#define LZ4_SECURECOPY(d,s,e)     { if (d<e) LZ4_WILDCOPY(d,s,e); }

	char* cpyEnd = dst + count - STEPSIZE;

	LZ4_WILDCOPY(dst, src, cpyEnd);
}

void TestRLE()
{
	int32 compRawTime = 0;
	int32 compStreamTime = 0;

	int32 decompRawTime = 0;
	int32 decompStreamTime = 0;

	int32 copyRawTime = 0;

	FileStream fs(L"testMath1.dat");
	char* buffer = new char[(int32)fs.getLength()];
	fs.Read(buffer, fs.getLength());
	fs.Close();

	int32 compressedSize = rleEvalCompressedSize(buffer, (int32)fs.getLength());

	compStreamTime = GetTickCount();
	MemoryOutStream compressedStream(compressedSize);
	rleCompress(buffer, (int32)fs.getLength(), &compressedStream);
	assert(compressedStream.getLength() == compressedSize);
	compStreamTime = GetTickCount() - compStreamTime;

	printf("RLE Comp Stream: %d,%d\n", compStreamTime, compressedSize);

	compRawTime = GetTickCount();
	char* compressedData = new char[compressedSize];
	int ret = rleCompress(compressedData, compressedSize, buffer, (int32)fs.getLength());
	assert(ret == compressedSize);
	compRawTime = GetTickCount() - compRawTime;

	printf("RLE Comp Buffer: %d\n", compStreamTime);



	//////////////////////////////////////////////////////////////////////////

	int32 decompSize = rleEvalDecompressedSize(compressedData, compressedSize);
	assert(decompSize == fs.getLength());
	
	decompRawTime = GetTickCount();
	char* decompressedData = new char[decompSize];
	ret = rleDecompress(decompressedData, decompSize, compressedData, compressedSize);
	assert(ret == decompSize);
	decompRawTime = GetTickCount() - decompRawTime;
	printf("RLE Decomp Buffer: %d\n", decompRawTime);

	CheckEqual(buffer, decompressedData, decompSize);

	memset(decompressedData, decompSize, 0);

	decompStreamTime = GetTickCount();
	MemoryStream ms(compressedData, compressedSize);
	VirtualStream vs1(&ms);
	VirtualStream vs2(&vs1);
	VirtualStream vs3(&vs2);
	VirtualStream vs4(&vs3);
	VirtualStream vs5(&vs4);
	BufferedStreamReader bsr(&vs5);
	ret = rleDecompress(decompressedData, decompSize, &bsr);
	assert(ret == decompSize);
	decompStreamTime = GetTickCount() - decompStreamTime;
	printf("RLE Decomp Stream: %d\n", decompStreamTime);

	CheckEqual(buffer, decompressedData, decompSize);


	{
		copyRawTime = GetTickCount();
		memcpy(decompressedData, buffer, fs.getLength());
		copyRawTime = GetTickCount() - copyRawTime;
		printf("Copy Buffer: %d\n", copyRawTime);
	}



	delete[] buffer;
	delete[] decompressedData;
	delete[] compressedData;
}
void TestLZ4()
{
	FileStream fs(L"testMath1.dat");
	int32 srcDataSize = (int32)fs.getLength();

	char* buffer = new char[srcDataSize];
	fs.Read(buffer, fs.getLength());
	fs.Close();

	
	char* compressed = new char[LZ4_COMPRESSBOUND(srcDataSize)];
	int32 t = GetTickCount();
	//int32 res = LZ4_compress(buffer, compressed, srcDataSize);
	int32 res = LZ4_compressHC2(buffer, compressed, srcDataSize,8);
	t = GetTickCount() - t;
	printf("LZ4 Comp: Time=%d, Size=%d\n", t, res);

	t = GetTickCount();
	char* decompressed = new char[srcDataSize];
	int32 res2 = LZ4_decompress_safe(compressed, decompressed, res, srcDataSize);
	t = GetTickCount() - t;
	printf("LZ4 Decomp Buffer: Time=%d\n", t);

	assert(res2 == srcDataSize);
	CheckEqual(buffer, decompressed, srcDataSize);

	delete[] decompressed;
	delete[] buffer;
	delete[] compressed;
}

void TestBufferStream()
{
	FileStream fs(L"Apoc3d.lib");
	char* buffer = new char[(int32)fs.getLength()];
	fs.Read(buffer, fs.getLength());
	fs.Close();


	MemoryStream ms(buffer, fs.getLength());
	BufferedStreamReader bsr(&ms);
	char* cmp = buffer;
	for (;;)
	{
		char temp[102400];
		int32 countToRead = Randomizer::NextExclusive(ARRAYSIZE(temp));
		int32 r = bsr.Read(temp, countToRead);
		CheckEqual<char>(cmp, temp, r);
		
		cmp += r;

		if (r < countToRead)
			break;
	}
	//char temp[102400];
	//bsr.Read(temp, 4);
	//CheckEqual<char>(cmp, temp, 4);
	//cmp += 4;

	//bsr.Read(temp, 1024);
	//CheckEqual<char>(cmp, temp, 1024);
	//cmp += 1024;

	//bsr.Read(temp, 4096);
	//CheckEqual<char>(cmp, temp, 4096);
	//cmp += 4096;

	//bsr.Read(temp, 4096);
	//CheckEqual<char>(cmp, temp, 4096);
	//cmp += 4096;

	//bsr.Read(temp, 128);
	//CheckEqual<char>(cmp, temp, 128);
	//cmp += 128;

	//bsr.Read(temp, 8192);
	//CheckEqual<char>(cmp, temp, 8192);
	//cmp += 8192;

	//bsr.Read(temp, 102400);
	//CheckEqual<char>(cmp, temp, 102400);
	//cmp += 102400;

	delete[] buffer;
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

	volatile DWORD start = GetTickCount();
	for (int i=0;i<count;i++)
	{
		Matrix::Multiply(result[i], set1[i], set2[i]);
	}
	volatile DWORD end = GetTickCount();

	DWORD t = end - start;
	std::wcout << t << L"\n";

	BinaryWriter* bw = new BinaryWriter(new FileOutStream(L"testMath1.dat"));
	bw->Write((char*)result, sizeof(Matrix) * count);
	delete bw;
}

void TestIni()
{
	String path;
	path.append(L"testIni.ini");

	FileLocation fl(path);// FileSystem::getSingleton().Locate(L"testIni.ini", FileLocateRule::Default);
	Configuration* config = IniConfigurationFormat::Instance.Load(fl);

	FileOutStream fs(L"iniOut.ini");
	IniConfigurationFormat::Instance.Save(config, &fs);

	delete config;
}
void TestXml()
{
	String path;
	path.append(L"testXml.xml");

	FileLocation fl(path);// FileSystem::getSingleton().Locate(L"testIni.ini", FileLocateRule::Default);
	Configuration* config = XMLConfigurationFormat::Instance.Load(fl);

	delete config;
}
void TestRandom()
{
	const int32 count = 1048576 * 64;
	int32* buf1 = new int32[count];

	int64 ta = GetTickCount();
	for (int32 i=0;i<count;i++)
	{
		buf1[i] = Randomizer::Next();
	}
	ta = GetTickCount() - ta;
	printf("RND1: %d\n", ta);

	delete[] buf1;

	/*int32 raw = 0x7fffffff;
	int32 sample = (int32)(raw & 0x7fffffffUL);
	double fltSample = sample / 2147483647.0;
	int32 rnd = (int32)(50 * fltSample);
	printf("RNDTest: %d\n", rnd);*/

}