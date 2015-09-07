#ifndef UT_TESTCOMMON_H
#define UT_TESTCOMMON_H

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <ctime>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#ifdef _DEBUG
#define MEM_BeginCheckPoint _CrtMemCheckpoint(&m_testBeginMemState);
#define MEM_EndCheckPoint	_CrtMemState testMemDifference; \
							_CrtMemCheckpoint(&m_testEndMemState); \
							Assert::IsTrue( !_CrtMemDifference(&testMemDifference, &m_testBeginMemState, &m_testEndMemState), L"Memory leaked" );
#define MEM_Variables _CrtMemState m_testBeginMemState; \
						_CrtMemState m_testEndMemState;
#else

#define MEM_BeginCheckPoint
#define MEM_EndCheckPoint
#define MEM_Variables
#endif

class MemoryService
{
public:
	MemoryService()
	{
		MEM_BeginCheckPoint;

		srand((unsigned int)time(0));

		int32 seed = 42353589;

		intDataCount = 8 + rand() % 16;
		intData = new int[intDataCount];
		for (int i = 0; i < intDataCount; i++)
			intData[i] = (i + seed) % intDataCount;

		ptrDataCount = 8 + rand() % 16;
		ptrData = new void*[ptrDataCount];
		for (int i = 0; i < ptrDataCount; i++)
			ptrData[i] = (void*)((i + seed) << 12);

		strDataCount = 8 + rand() % 16;
		stringData = new String[strDataCount];
		for (int i = 0; i < strDataCount; i++)
		{
			for (int j = 0; j < 32; j++)
			{
				wchar_t letter = (i + j + seed) % ('Z' - 'A') + 'A';
				stringData[i].append(1, letter);
			}
		}
	}
	~MemoryService()
	{
		delete[] intData;
		delete[] ptrData;
		delete[] stringData;

		MEM_EndCheckPoint;
	}

	const String& randomString() const { return stringData[Randomizer::NextExclusive(strDataCount)]; }

	int* intData;
	int32 intDataCount;
	void** ptrData;
	int32 ptrDataCount;
	String* stringData;
	int32 strDataCount;
private:
	MEM_Variables;
};

class NonMovableString
{
public:
	NonMovableString(const wchar_t* s)
	{
		size_t len = wcslen(s);
		m_content = new wchar_t[len+1]();
		wcscpy_s(m_content, len + 1, s);
	}

	~NonMovableString()
	{
		delete[] m_content;
		m_content = nullptr;
	}

	bool operator==(const String& o) { return wcscmp(o.c_str(), m_content) == 0; }
	bool operator==(const NonMovableString& o) { return wcscmp(o.m_content, m_content) == 0; }

	bool operator!=(const NonMovableString& o) { return !this->operator ==(o); }
	bool operator!=(const String& o) { return !this->operator ==(o); }

private:
	wchar_t* m_content;
};

#endif