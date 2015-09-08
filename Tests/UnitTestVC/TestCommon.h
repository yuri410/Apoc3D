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

class TrivialString final
{
public:
	TrivialString()
	{
		ZeroArray(m_content);
	}

	TrivialString(const wchar_t* s)
	{
		ZeroArray(m_content);

		size_t len = wcslen(s);

		wcscpy_s(m_content, countof(m_content), s);
	}

	~TrivialString()
	{ }

	bool operator==(const String& o) { return wcscmp(o.c_str(), m_content) == 0; }
	bool operator==(const TrivialString& o) { return wcscmp(o.m_content, m_content) == 0; }

	bool operator!=(const String& o) { return !this->operator ==(o); }
	bool operator!=(const TrivialString& o) { return !this->operator ==(o); }

	const wchar_t* c_str() const { return m_content; }
private:
	wchar_t m_content[128];
};

class NonMovableString final
{
public:
	NonMovableString() { }
	NonMovableString(const wchar_t* s)
	{
		size_t len = wcslen(s);
		m_content = new wchar_t[len + 1]();
		wcscpy_s(m_content, len + 1, s);
	}
	NonMovableString(const NonMovableString& o)
		: NonMovableString(o.m_content) { }

	~NonMovableString()
	{
		delete[] m_content;
		m_content = nullptr;
	}

	NonMovableString& operator=(const NonMovableString& o)
	{
		if (this != &o)
		{
			delete[] m_content;
			size_t len = wcslen(o.m_content);
			m_content = new wchar_t[len + 1]();
			wcscpy_s(m_content, len + 1, o.m_content);
		}
		return *this;
	}

	bool operator==(const String& o) const
	{
		if (m_content == nullptr && o.empty())
			return true;
		return wcscmp(o.c_str(), m_content) == 0; 
	}
	bool operator==(const NonMovableString& o) const
	{
		if (m_content == nullptr && o.m_content == nullptr)
			return true;
		return wcscmp(o.m_content, m_content) == 0; 
	}

	bool operator!=(const String& o) const { return !this->operator ==(o); }
	bool operator!=(const NonMovableString& o) const { return !this->operator ==(o); }

	const wchar_t* c_str() const { return m_content ? m_content : L""; }

private:
	wchar_t* m_content = nullptr;
};

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
		tstringData = new TrivialString[strDataCount];
		cstringData = new NonMovableString[strDataCount];
		for (int i = 0; i < strDataCount; i++)
		{
			for (int j = 0; j < 32; j++)
			{
				wchar_t letter = (i + j + seed) % ('Z' - 'A') + 'A';
				stringData[i].append(1, letter);
			}

			tstringData[i] = stringData[i].c_str();
			cstringData[i] = stringData[i].c_str();
		}
	}
	~MemoryService()
	{
		delete[] intData;
		delete[] ptrData;
		delete[] stringData;
		delete[] tstringData;
		delete[] cstringData;

		MEM_EndCheckPoint;
	}

	const String& randomString() const { return stringData[Randomizer::NextExclusive(strDataCount)]; }
	const TrivialString& randomTString() const { return tstringData[Randomizer::NextExclusive(strDataCount)]; }
	const NonMovableString& randomCString() const { return cstringData[Randomizer::NextExclusive(strDataCount)]; }

	int* intData;
	int32 intDataCount;
	void** ptrData;
	int32 ptrDataCount;
	String* stringData;
	TrivialString* tstringData;
	NonMovableString* cstringData;
	int32 strDataCount;

private:
	MEM_Variables;
};


namespace Microsoft
{
	namespace VisualStudio
	{
		namespace CppUnitTestFramework
		{
			template<> static std::wstring ToString<Vector3>(const Vector3& t) { return t.ToTextString(1); }
			template<> static std::wstring ToString<Point>(const Point& t) { return StringUtils::IntToString(t.X) + L", " + StringUtils::IntToString(t.Y); }

			template<> static std::wstring ToString<TrivialString>(const TrivialString& t) { return t.c_str(); }
			template<> static std::wstring ToString<NonMovableString>(const NonMovableString& t) { return t.c_str(); }
		}
	}
}

#endif