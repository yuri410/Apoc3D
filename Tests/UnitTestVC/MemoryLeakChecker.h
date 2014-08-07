#ifndef UT_MEMORYLEAKCHECKER_H
#define UT_MEMORYLEAKCHECKER_H

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

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

#endif