#ifndef UT_MEMORYLEAKCHECKER_H
#define UT_MEMORYLEAKCHECKER_H

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
			for (int j = 0; j < 32;j++)
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

	int* intData;
	int32 intDataCount;
	void** ptrData;
	int32 ptrDataCount;
	String* stringData;
	int32 strDataCount;
private:
	MEM_Variables;
};

#endif