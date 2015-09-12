#include <Windows.h>

#include "apoc3d/Engine.h"
#include "apoc3d/Config/IniConfigurationFormat.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"
#include "apoc3d/IOLib/TaggedData.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/IOLib/TextData.h"
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
#include "apoc3d/Math/Math.h"
#include "apoc3d/Utility/Compression.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Utility/StringTable.h"
#include "apoc3d/Library/lz4.h"
#include "apoc3d/Library/lz4hc.h"
#include "apoc3d/Library/tinyxml.h"
#include "apoc3d/Vfs/File.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/FileLocateRule.h"
#include "apoc3d/Vfs/PathUtils.h"

#include <iostream>
#include <chrono>
#include <vector>
//#include <functional>

using namespace Apoc3D;
using namespace Apoc3D::Config;
using namespace Apoc3D::Math;
using namespace Apoc3D::IO;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;

void TestRLE();
void TestMath();
void TestBufferStream();
void TestLZ4();

void TestIni();
void TestXml();

void TestRandom();

void TestIterator2();

void TestHalfFloat();

void main()
{
	setlocale(LC_CTYPE, ".ACP");

	//String s = StringUtils::ToString(0.14159265f,8);
	//std::wcout << s.c_str() << L"\n";
	//std::wcout << std::numeric_limits<float>::digits10+1 << L"\n";


	TestIterator2();

	return;

	FileSystem::Initialize();
	wchar_t workingDir[260];
	GetCurrentDirectory(260, workingDir);
	FileSystem::getSingleton().AddWrokingDirectory(workingDir);
	
	List<String> items;
	FileSystem::getSingleton().ListDirectoryFiles(L"", items);

	TestMath();
	TestBufferStream();
	//TestRLE();
	//TestLZ4();
	TestIni();
	TestXml();

	//TestRandom();
	//TestHalfFloat();
	
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
		memcpy(decompressedData, buffer, (size_t)fs.getLength());
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

	{
		MemoryStream ms(buffer, fs.getLength());
		BufferedStreamReader bsr(&ms);
		char* cmp = buffer;
		for (;;)
		{
			char temp[8192];
			int32 countToRead = Randomizer::NextExclusive(ARRAYSIZE(temp));
			int32 r = bsr.Read(temp, countToRead);
			CheckEqual<char>(cmp, temp, r);

			cmp += r;

			if (r < countToRead)
				break;
		}
	}

	{
		MemoryStream ms(buffer, fs.getLength());
		BufferedStreamReader bsr(&ms);
		FileStream fs2(L"Apoc3d.lib");

		for (;;)
		{
			char temp[8192];
			char temp2[8192];

			int32 countToRead = Randomizer::NextExclusive(countof(temp));
			int32 r1 = bsr.Read(temp, countToRead);
			int32 r2 = (int32)fs2.Read(temp2, (int64)countToRead);
			assert(r1 == r2);

			CheckEqual<char>(temp2, temp, r1);

			if (r1 < countToRead)
				break;
		}
	}

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

	FileOutStream fs(L"testMath1.dat");
	BinaryWriter bw(&fs, false);
	bw.Write((char*)result, sizeof(Matrix) * count);
}

void TestIni()
{
	String path;
	path.append(L"testIni.ini");

	Configuration config;
	IniConfigurationFormat::Instance.Load(FileLocation(path), &config);

	IniConfigurationFormat::Instance.Save(&config, FileOutStream(L"iniOut.ini"));
}
void TestXml()
{
	String path;
	path.append(L"testXml.xml");

	Configuration config(path);
	XMLConfigurationFormat::Instance.Load(FileLocation(path), &config);

	
}
void TestRandom()
{
	const int32 count = 1048576 * 64;

	int32 buckets[3] = { 0 };
	float rates[3] = { 0 };

	for (int32 i=0;i<count;i++)
	{
		int32 v = Randomizer::NextExclusive(3);
		buckets[v]++;
	}

	int32 total = 0;
	for (int32 v : buckets)
		total += v;
	
	for (int32 i = 0; i < countof(rates); i++)
		rates[i] = buckets[i] / (float)total;

	printf("Random Distribution: %.2f, %.2f, %.2f\n", rates[0]*10, rates[1]*10, rates[2]*10);

}

template<typename T>
NO_INLINE void OptimizationBlocker(volatile const T& e) 
{
	(void)e;
	//volatile char r = *reinterpret_cast<const char*>(&e);
	//volatile char r[sizeof(T)];
	//for (int32 i = 0; i < sizeof(T); i++)
		//r[i] = *(reinterpret_cast<const char*>(&e) + i);
}
template<typename T, typename S>
NO_INLINE void OptimizationBlocker(volatile const T& e, volatile const S& f)
{
	(void)e;
	(void)f;
}

NO_INLINE int64 getTimeDiff(const volatile std::chrono::high_resolution_clock::time_point& t1,
	const volatile std::chrono::high_resolution_clock::time_point& t2)
{
	using namespace std::chrono;

	return duration_cast<milliseconds>(const_cast<high_resolution_clock::time_point&>(t2)-const_cast<high_resolution_clock::time_point&>(t1)).count();
}

template <typename T>
struct LargerListIter
{
	explicit LargerListIter(List<T>& list)
		: m_list(list) { }

	LargerListIter& operator++()
	{
		if (m_index < 0)
			return *this;

		m_index++;

		if (m_index >= m_list.getCount())
			m_index = -1;

		return *this;
	}
	//LargerListIter operator++(int) { LargerListIter result = *this; ++(*this); return result; }

	bool operator==(const LargerListIter& rhs) const { return &m_list == &rhs.m_list && m_index == rhs.m_index; }
	bool operator!=(const LargerListIter& rhs) const { return &m_list != &rhs.m_list || m_index != rhs.m_index; }

	const T& operator*() const { return m_list[m_index]; }

	static LargerListIter ssend(List<T>& list) { return LargerListIter(list, -1); }
private:
	LargerListIter(List<T>& list, int32 idx)
		: m_list(list), m_index(idx) { }

	List<T>& m_list;
	int32 m_index = 0;
};


bool TestCond(int32& r) { return r > 100 && r < 10240; }

void TestIterator2()
{
	using namespace std::chrono;

	const int Count = 100000;
	const int Iterations = 1000;

	List<int> test3(Count);
	std::vector<int> test2;

	for (int32 i = 0; i < Count; i++)
	{
		test3.Add(i);
		test2.push_back(i);
	}

	volatile int64 ifTime;
	volatile int64 cndTime;

	//if (1)
	//{
	//	{
	//		volatile auto t1 = high_resolution_clock::now();
	//		for (int k = 0; k < Iterations; k++)
	//		{
	//			for (auto e : ConditionalAccessor<int32, int32, TestCond>(test3.begin(), test3.end()))
	//				OptimizationBlocker(e);
	//		}
	//		volatile auto t2 = high_resolution_clock::now();
	//		cndTime = getTimeDiff(t1, t2);
	//	}
	//	{
	//		volatile auto t1 = high_resolution_clock::now();
	//		for (int k = 0; k < Iterations; k++)
	//		{
	//			for (auto& e : test3)
	//				if (TestCond(e))
	//					OptimizationBlocker(e);
	//		}
	//		volatile auto t2 = high_resolution_clock::now();
	//		ifTime = getTimeDiff(t1, t2);
	//	}
	//	printf("ConditionalAccessor:\tif=%lld,\tcond=%lld\n", ifTime, cndTime);
	//}

	if (1)
	{
		{
			volatile auto t1 = high_resolution_clock::now();
			for (int k = 0; k < Iterations; k++)
			{
				for (auto e : test3)
					OptimizationBlocker(e);
			}
			volatile auto t2 = high_resolution_clock::now();
			cndTime = getTimeDiff(t1, t2);
		}
		{
			volatile auto t1 = high_resolution_clock::now();
			for (int k = 0; k < Iterations; k++)
			{
				for (auto& e : test2)
					OptimizationBlocker(e);
			}
			volatile auto t2 = high_resolution_clock::now();
			ifTime = getTimeDiff(t1, t2);
		}
		printf("std:%lld\napoc:%lld\n", ifTime, cndTime);
	}
}


void TestHalfFloat()
{
	using namespace std::chrono;

	const int Iterations = 10000000;

	volatile int64 c1;
	volatile int64 c2;

	{
		volatile auto t1 = high_resolution_clock::now();
		for (int i = 0; i < Iterations; i++)
		{
			uint iv = Math::R16ToR32I((uint16)(i % 65536));
			OptimizationBlocker(iv);
		}
		volatile auto t2 = high_resolution_clock::now();
		c1 = getTimeDiff(t1, t2);
	}
	{
		volatile auto t1 = high_resolution_clock::now();
		for (int i = 0; i < Iterations; i++)
		{
			uint16 iv = Math::R32ToR16I(i);
			OptimizationBlocker(iv);
		}
		volatile auto t2 = high_resolution_clock::now();
		c2 = getTimeDiff(t1, t2);
	}
	printf("HalfFloat: %lld,%lld\n", c1, c2);
}