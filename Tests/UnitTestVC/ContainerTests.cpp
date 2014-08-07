#include "MemoryLeakChecker.h"
#include <ctime>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestVC
{		
	TEST_CLASS(ListTest)
	{
		int* m_intData;
		int32 m_intDataCount;
		void** m_ptrData;
		int32 m_ptrDataCount;
		String* m_stringData;
		int32 m_strDataCount;
		MEM_Variables;

	public:
		
		ListTest()
		{
			MEM_BeginCheckPoint;

			srand((unsigned int)time(0));

			m_intDataCount = 8 + rand() % 16;
			m_intData = new int[m_intDataCount];
			for (int i = 0; i < m_intDataCount; i++)
				m_intData[i] = rand();

			m_ptrDataCount = 8 + rand() % 16;
			m_ptrData = new void*[m_ptrDataCount];
			for (int i = 0; i < m_ptrDataCount; i++)
				m_ptrData[i] = (void*)rand();

			m_strDataCount = 8 + rand() % 16;
			m_stringData = new String[m_strDataCount];
			for (int i = 0; i < m_strDataCount; i++)
			{
				wchar_t letter = rand() % ('Z' - 'A') + 'A';
				m_stringData[i].append(1, letter);
			}
		}
		~ListTest()
		{
			delete[] m_intData;
			delete[] m_ptrData;
			delete[] m_stringData;

			MEM_EndCheckPoint;
		}

		TEST_METHOD(ListReverse)
		{
			List<int> subject;

			subject.Add(0);
			subject.Add(1);
			subject.Add(2);
			subject.Add(3);

			subject.Reverse();

			Assert::AreEqual(subject[0], 3);
			Assert::AreEqual(subject[1], 2);
			Assert::AreEqual(subject[2], 1);
			Assert::AreEqual(subject[3], 0);

			subject.Reverse();
			subject.Add(4);
			subject.Reverse();
			Assert::AreEqual(subject[0], 4);
			Assert::AreEqual(subject[1], 3);
			Assert::AreEqual(subject[2], 2);
			Assert::AreEqual(subject[3], 1);
			Assert::AreEqual(subject[4], 0);

		}

		TEST_METHOD(ListSort)
		{
			List<int> subject(100);
			int counter[1000];
			memset(counter, 0, sizeof(counter));

			for (int i = 0; i < 100; i++)
			{
				int val = Randomizer::NextExclusive(1000);
				subject.Add(val);
				counter[val]++;
			}

			subject.Sort();


			for (int i = 0; i < subject.getCount() - 1; i++)
			{
				if (subject[i + 1] < subject[i])
				{
					Assert::Fail();
				}
			}

			int counter2[1000];
			memset(counter2, 0, sizeof(counter2));
			for (int i = 0; i < subject.getCount(); i++)
			{
				counter2[subject[i]]++;
			}

			Assert::IsTrue(memcmp(counter, counter2, sizeof(counter)) == 0);
		}

		TEST_METHOD(ListSort2)
		{
			List<int> subject(100);
			int counter[1000];
			memset(counter, 0, sizeof(counter));

			for (int i = 0; i < 100; i++)
			{
				int val = Randomizer::NextExclusive(1000);
				subject.Add(val);
				counter[val]++;
			}

			subject.Sort<OrderComparer>();


			for (int i = 0; i < subject.getCount() - 1; i++)
			{
				if (subject[i + 1] < subject[i])
				{
					Assert::Fail();
				}
			}

			int counter2[1000];
			memset(counter2, 0, sizeof(counter2));
			for (int i = 0; i < subject.getCount(); i++)
			{
				counter2[subject[i]]++;
			}

			Assert::IsTrue(memcmp(counter, counter2, sizeof(counter)) == 0);
		}

		TEST_METHOD(ListIterator)
		{
			List<int> subject;

			subject.Add(0);
			subject.Add(1);
			subject.Add(2);

			List<int> result;
			for (int e : subject)
			{
				result.Add(e);
			}

			Assert::AreEqual(3, result.getCount());
			if (result.getCount() == 3)
			{
				Assert::AreEqual(0, result[0]);
				Assert::AreEqual(1, result[1]);
				Assert::AreEqual(2, result[2]);
			}

			subject.Clear();

			for (auto e : subject)
			{
				Assert::Fail();
			}
		}

		TEST_METHOD(ListAddMult)
		{
			const int preData[] = { 44, 55, 66, 77 };

			for (int32 i = 0; i < countof(preData); i++)
			{
				List<int> s1;

				for (int32 j = 0; j <= i; j++)
				{
					s1.Add(preData[j]);
				}

				s1.AddArray(m_intData, m_intDataCount);

				for (int32 j = 0; j < s1.getCount(); j++)
				{
					if (j <= i)
						Assert::AreEqual(preData[j], s1[j]);
					else
						Assert::AreEqual(m_intData[j - i - 1], s1[j]);
				}

				Assert::AreEqual(i + m_intDataCount + 1, s1.getCount());
			}

			List<int> s2;
			s2.AddArray(preData);

			Assert::AreEqual(countof(preData), s2.getCount());
			for (int32 i = 0; i < s2.getCount(); i++)
			{
				Assert::AreEqual(preData[i], s2[i]);
			}
		}

		//////////////////////////////////////////////////////////////////////////

		TEST_METHOD(ListOrderInt)
		{
			_TestOrder<int>(m_intData, m_intDataCount);
		}

		TEST_METHOD(ListOrderVoid)
		{
			_TestOrder<void*>(m_ptrData, m_ptrDataCount);
		}

		TEST_METHOD(ListOrderString)
		{
			_TestOrder<String>(m_stringData, m_strDataCount);
		}

		//////////////////////////////////////////////////////////////////////////

		TEST_METHOD(ListClearInt)
		{
			int data[5] = { 1, 2, 3, 4, 5 };
			_TestClear<int>(data, 5, 100, 101);
		}

		TEST_METHOD(ListClearVoid)
		{
			void* data[5] = { (void*)1, (void*)2, (void*)3, (void*)4, (void*)5 };
			_TestClear<void*>(data, 5, (void*)100, (void*)101);
		}

		TEST_METHOD(ListClearString)
		{
			String data[5] = { L"a", L"b", L"c", L"d", L"e" };
			_TestClear<String>(data, 5, L"x", L"y");
		}

		//////////////////////////////////////////////////////////////////////////

		TEST_METHOD(ListCopyInt)
		{
			int data[5] = { 1, 2, 3, 4, 5 };
			_TestCopy<int>(data, 5, 100, 101);
		}

		TEST_METHOD(ListCopyVoid)
		{
			void* data[5] = { (void*)1, (void*)2, (void*)3, (void*)4, (void*)5 };
			_TestCopy<void*>(data, 5, (void*)100, (void*)101);
		}

		TEST_METHOD(ListCopyString)
		{
			String data[5] = { L"a", L"b", L"c", L"d", L"e" };
			_TestCopy<String>(data, 5, L"x", L"y");
		}

		//////////////////////////////////////////////////////////////////////////

		TEST_METHOD(ListIteratorInt)
		{
			_TestIterator<int>(m_intData, m_intDataCount);
		}

		TEST_METHOD(ListIteratorVoid)
		{
			_TestIterator<void*>(m_ptrData, m_ptrDataCount);
		}

		TEST_METHOD(ListIteratorString)
		{
			_TestIterator<String>(m_stringData, m_strDataCount);
		}

		//////////////////////////////////////////////////////////////////////////

		TEST_METHOD(ListRemoveInt)
		{
			int data[5] = { 1, 2, 3, 4, 5 };
			_TestRemove<int>(data, 5, 100);
		}

		TEST_METHOD(ListRemoveVoid)
		{
			void* data[5] = { (void*)1, (void*)2, (void*)3, (void*)4, (void*)5 };
			_TestRemove<void*>(data, 5, (void*)100);
		}

		TEST_METHOD(ListRemoveString)
		{
			String data[5] = { L"a", L"b", L"c", L"d", L"e" };
			_TestRemove<String>(data, 5, L"missing");
		}

		
		//////////////////////////////////////////////////////////////////////////

	private:

		template<typename S>
		void _TestOrder(const S* data, int32 dataCount)
		{
			List<S> subject;
			for (int32 i = 0; i < dataCount; i++)
			{
				Assert::AreEqual(i, subject.getCount());
				subject.Add(data[i]);
				Assert::AreEqual(data[i], subject[i]);
			}

			Assert::AreEqual(dataCount, subject.getCount());

			for (int32 i = dataCount - 1; i >= 0; i--)
			{
				Assert::AreEqual(subject[i], data[i]);
				subject.RemoveAt(subject.getCount() - 1);
			}

			Assert::AreEqual(0, subject.getCount());

		}

		template<typename S>
		void _TestCopy(const S* data, int32 dataCount, const S& a, const S& b)
		{
			List<S> subject;
			for (int32 i = 0; i < dataCount; i++)
			{
				Assert::AreEqual(i, subject.getCount());
				subject.Add(data[i]);
			}

			List<S> testList2(subject);
			List<S> testList3; testList3.Add(a); testList3.Add(b);

			testList3 = subject;

			Assert::AreEqual(dataCount, testList3.getCount());

			for (int32 i = 0; i < dataCount; i++)
			{
				Assert::AreEqual(testList2[i], subject[i]);
				Assert::AreEqual(testList3[i], subject[i]);
				Assert::AreEqual(data[i], subject[i]);

			}

		}

		template<typename S>
		void _TestClear(const S* data, int32 dataCount, const S& a, const S& b)
		{
			List<S> subject;
			for (int32 i = 0; i < dataCount; i++)
			{
				Assert::AreEqual(i, subject.getCount());
				subject.Add(data[i]);
			}

			List<S> testList2(subject);
			List<S> testList3; testList3.Add(a); testList3.Add(b);

			testList3 = subject;


			subject.Clear();
			testList2.Clear();
			testList3.Clear();

			Assert::AreEqual(0, subject.getCount());
			Assert::AreEqual(0, testList2.getCount());
			Assert::AreEqual(0, testList3.getCount());
		}

		template<typename S>
		void _TestIterator(const S* data, int32 dataCount)
		{
			List<S> subject;

			for (int32 i = 0; i < dataCount; i++)
			{
				subject.Add(data[i]);
			}

			S* iter1 = subject.begin();
			S* iter2 = iter1;
			int counter = 0;
			do
			{
				Assert::IsTrue(data[counter] == *iter2);
				Assert::IsTrue(*iter1 == data[counter]);

				Assert::IsTrue(iter1 == iter2);

				S* iter3 = iter2;
				Assert::IsTrue(iter1 == iter3);

				iter1++;
				++iter2;
				counter++;
			} while (iter1 != subject.end());
		}


		template<typename S>
		void _TestRemove(const S* data, int32 dataCount, const S& dataNotFound)
		{
			List<S> subject;

			for (int32 i = 0; i < dataCount; i++)
			{
				subject.Add(data[i]);
			}

			bool result = subject.Remove(data[dataCount - 1]);
			Assert::IsTrue(result);
			Assert::AreEqual(subject.getCount(), dataCount - 1);
			Assert::AreEqual(subject[subject.getCount() - 1], data[dataCount - 2]);
			
			Assert::IsFalse(subject.Remove(dataNotFound));
			Assert::AreEqual(subject.getCount(), dataCount - 1);

			subject.RemoveAt(subject.getCount() - 1);
			Assert::AreEqual(subject.getCount(), dataCount - 2);
			Assert::AreEqual(subject[subject.getCount() - 1], data[dataCount - 3]);
			
			subject.RemoveRange(1, subject.getCount() - 1);
			Assert::AreEqual(subject.getCount(), 1);
			Assert::AreEqual(subject[0], data[0]);

			List<S> subject2;
			for (int32 i = 0; i < dataCount; i++)
			{
				subject2.Add(data[i]);
			}

			subject2.RemoveRange(1, subject2.getCount() - 1);
			Assert::AreEqual(subject2.getCount(), 1);
			Assert::AreEqual(subject2[0], data[0]);

		}
		
	};


	TEST_CLASS(HashMapTest)
	{
		MEM_Variables;
	public:
		HashMapTest()
		{
			MEM_BeginCheckPoint;

		}
		~HashMapTest()
		{
			MEM_EndCheckPoint;
		}

		TEST_METHOD(HashMapIterator)
		{
			HashMap<int, int> subject;

			subject.Add(0, 2);
			subject.Add(1, 1);
			subject.Add(2, 0);

			List<int> result;

			for (int e : subject.getKeyAccessor())
			{
				result.Add(e);
			}

			Assert::AreEqual(3, result.getCount());
			if (result.getCount() == 3)
			{
				Assert::AreEqual(0, result[0]);
				Assert::AreEqual(1, result[1]);
				Assert::AreEqual(2, result[2]);
			}

			result.Clear();

			for (int e : subject.getValueAccessor())
			{
				result.Add(e);
			}
			Assert::AreEqual(3, result.getCount());
			if (result.getCount() == 3)
			{
				Assert::AreEqual(2, result[0]);
				Assert::AreEqual(1, result[1]);
				Assert::AreEqual(0, result[2]);
			}

			result.Clear();

			for (auto e : subject)
			{
				result.Add(e.Key);
				result.Add(e.Value);
			}

			Assert::AreEqual(6, result.getCount());
			if (result.getCount() == 6)
			{
				Assert::AreEqual(0, result[0]);
				Assert::AreEqual(2, result[1]);
				Assert::AreEqual(1, result[2]);
				Assert::AreEqual(1, result[3]);
				Assert::AreEqual(2, result[4]);
				Assert::AreEqual(0, result[5]);
			}

			subject.Clear();

			for (auto e : subject)
			{
				Assert::Fail();
			}
		}

		TEST_METHOD(HashMapCopy)
		{
			HashMap<String, String> m1(10);

			HashMap<String, String> m2 = m1;
			m2.Add(L"1", L"");
			m2.Add(L"sssssssssssssssssssssssssssssssss", L"llllllllllllllllllllllllllllllll");
			m2.Add(L"s", L"dsa");

			HashMap<String, String> m3 = m2;
			m3.Clear();


			HashMap<String, String> m4;

			HashMap<String, String> m5;
			m5 = m4;
			m5.Add(L"1", L"");
			m5.Add(L"sssssssssssssssssssssssssssssssss", L"llllllllllllllllllllllllllllllll");
			m5.Add(L"s", L"dsa");

			m5.Clear();

			HashMap<String, String> m6 = m5;
		}

		TEST_METHOD(HashMapCount)
		{
			HashMap<String, String> m2;
			m2.Add(L"1", L"");
			m2.Add(L"sssssssssssssssssssssssssssssssss", L"llllllllllllllllllllllllllllllll");
			m2.Add(L"s", L"dsa");

			Assert::AreEqual(3, m2.getCount());

			m2.Remove(L"1");
			Assert::AreEqual(2, m2.getCount());

			m2.Remove(L"s");
			Assert::AreEqual(1, m2.getCount());

			m2.Add(L"asdsa", L"da");
			Assert::AreEqual(2, m2.getCount());

			m2.Add(L"sdasd", L"f");
			Assert::AreEqual(3, m2.getCount());

			m2.Add(L"sdsdsaf", L"s");
			Assert::AreEqual(4, m2.getCount());

			m2.Remove(L"sssssssssssssssssssssssssssssssss"); 
			Assert::AreEqual(3, m2.getCount());


			HashMap<String, String> m3 = m2;
			Assert::AreEqual(3, m3.getCount());

			m3.Clear();
			Assert::AreEqual(0, m3.getCount());

		}

		TEST_METHOD(HashMapAddRemove)
		{
			const int amount = 1000;
			String srcs[amount];
			bool added[amount] = { 0 };
			for (int32 i = 0; i < amount;i++)
			{
				srcs[i] = StringUtils::IntToString(i);
			}

			HashMap<String, int> m2;
			m2.Resize(amount);

			for (int32 k = 0; k < 100;k++)
			{
				for (int32 i = 0; i < amount; i++)
				{
					if ((Randomizer::Next() & 1) == 0)
					{
						if (added[i])
						{
							m2.Remove(srcs[i]);
							added[i] = false;
						}
						else
						{
							m2.Add(srcs[i], i);
							added[i] = true;
						}
					}
				}
			}

			for (auto& e : m2)
			{
				Assert::IsTrue(added[e.Value]);
				Assert::AreEqual(srcs[e.Value], e.Key);
			}

			for (auto e = m2.GetEnumerator(); e.MoveNext();)
			{
				Assert::IsTrue(added[e.getCurrentValue()]);
				Assert::AreEqual(srcs[e.getCurrentValue()], e.getCurrentKey());
				
			}
		}

	};


	TEST_CLASS(HashSetTest)
	{
		MEM_Variables;
	public:
		HashSetTest()
		{
			MEM_BeginCheckPoint;

		}
		~HashSetTest()
		{
			MEM_EndCheckPoint;
		}


		TEST_METHOD(HashSetAddRemove)
		{
			const int amount = 1000;
			String srcs[amount];
			bool added[amount] = { 0 };
			for (int32 i = 0; i < amount; i++)
			{
				srcs[i] = StringUtils::IntToString(i);
			}

			HashSet<String> m2;
			m2.Resize(amount);

			for (int32 k = 0; k < 100; k++)
			{
				for (int32 i = 0; i < amount; i++)
				{
					if ((Randomizer::Next() & 1) == 0)
					{
						if (added[i])
						{
							m2.Remove(srcs[i]);
							added[i] = false;
						}
						else
						{
							m2.Add(srcs[i]);
							added[i] = true;
						}
					}
				}
			}

			for (const auto& e : m2)
			{
				int32 idx = StringUtils::ParseInt32(e);
				Assert::IsTrue(added[idx]);
			}
			for (auto e = m2.GetEnumerator(); e.MoveNext();)
			{
				int32 idx = StringUtils::ParseInt32(e.getCurrent());
				Assert::IsTrue(added[idx]);
			}
		}

	};
}